#include "Timer.hpp"
#include <functional>

bool operator<(const TimerNode &lhs, const TimerNode &rhs)
{
    return lhs.expires_ < rhs.expires_;
}

Timer::Timer(EventLoop *loop)
    : loop_(loop),
      timers_(Comp_),
      channel_(new Channel(loop_, timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)))
{
    channel_->setReadCallback(std::bind(&Timer::handleRead, this));
    channel_->enableRead();
}

unsigned int Timer::addTimer(TimerCallback cb, TimeStamp ts, MilliSeconds interval)
{
    unsigned int id = getNewId();
    TimerNodePtr node(new TimerNode(id, cb, ts, interval));

    loop_->runInLoop(std::bind(&Timer::addTimerInLoop, this, id, node));
    return id;
}

void Timer::addTimerInLoop(unsigned int id, TimerNodePtr node)
{
    timerIdx_[id] = node;
    if (insert(node))
    {
        adjust(node);
    }
}

void Timer::cancel(unsigned int id)
{
    loop_->runInLoop(std::bind(&Timer::cancelInLoop, this, id));
}

void Timer::cancelInLoop(unsigned int id)
{
    auto node = timerIdx_[id];
    timers_.erase(node);
    timerIdx_.erase(id);
}

void Timer::handleRead()
{
    LOG_INFO << "Timer::handleRead";
    uint64_t data;
    ssize_t n = ::read(channel_->fd(), &data, sizeof data);
    assert(sizeof data == n);
    TimeStamp current = Clock::now();
    TimerNodePtr temp(new TimerNode(-1, NULL, current, MilliSeconds(0)));
    // temp.expires_ = current;
    auto it = timers_.upper_bound(temp);
    std::vector<TimerNodePtr> ticks(timers_.begin(), it);
    for (int i = 0; i < ticks.size(); ++i)
    {
        ticks[i]->cb_();
        timers_.erase(ticks[i]);
        if (ticks[i]->interval_.count() > 0)
        {
            ticks[i]->expires_ += ticks[i]->interval_;
            timers_.emplace(ticks[i]);
        }
        else
        {
            timerIdx_.erase(ticks[i]->id_);
        }
    }
    if (!timers_.empty())
    {
        adjust(*timers_.begin());
    }
}

bool Timer::insert(TimerNodePtr node)
{
    if (timers_.empty() || node < *(timers_.begin()))
    {
        timers_.emplace(node);
        return true;
    }
    timers_.emplace(node);
    return false;
}

void Timer::adjust(TimerNodePtr node)
{

    itimerspec new_val;
    memset(&new_val, 0, sizeof new_val);
    NanoSeconds duration = node->expires_ - Clock::now();
    int64_t val = duration.count();
    timespec ts;
    ts.tv_sec = val / NanoSecondsPerSecond;
    ts.tv_nsec = val % NanoSecondsPerSecond;
    new_val.it_value = ts;
    timerfd_settime(channel_->fd(), 0, &new_val, NULL);
}