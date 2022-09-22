#include "Timer.hpp"
#include <functional>

bool operator<(const TimerNode &lhs, const TimerNode &rhs)
{
    return lhs.expires_ < rhs.expires_;
}

Timer::Timer(EventLoop *loop)
    : loop_(loop),
      channel_(new Channel(loop_, timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)))
{
    channel_->setReadCallback(std::bind(&Timer::handleRead, this));
    channel_->enableRead();
}

TimerNode Timer::addTimer(TimerCallback cb, TimeStamp ts, MilliSeconds interval)
{
    TimerNode node(cb, ts, interval);
    loop_->runInLoop(std::bind(&Timer::addTimerInLoop, this, cb, ts, interval));
    return node;
}

void Timer::addTimerInLoop(TimerCallback cb, TimeStamp ts, MilliSeconds interval)
{
    TimerNode node(cb, ts, interval);
    if (insert(node))
    {
        adjust(node);
    }
}

void Timer::cancel(const TimerNode &node)
{
    loop_->runInLoop(std::bind(&Timer::cancelInLoop, this, node));
}

void Timer::cancelInLoop(const TimerNode node)
{
    timers_.erase(node);
}

void Timer::handleRead()
{
    uint64_t data;
    ssize_t n = ::read(channel_->fd(), &data, sizeof data);
    assert(sizeof data == n);
    TimeStamp current = Clock::now();
    TimerNode temp(NULL, current, MilliSeconds(0));
    // temp.expires_ = current;
    auto it = timers_.upper_bound(temp);
    std::vector<TimerNode> ticks(timers_.begin(), it);
    for (auto it_ = ticks.begin(); it_ != ticks.end(); ++it_)
    {
        it_->cb_();
        timers_.erase(*it_);
        if (it_->interval_.count() > 0)
        {
            it_->expires_ += it_->interval_;
            timers_.emplace(*it_);
        }
    }
    adjust(*timers_.begin());
}

bool Timer::insert(const TimerNode &node)
{
    if (timers_.empty() || node < *(timers_.begin()))
    {
        timers_.emplace(node);
        return true;
    }
    timers_.emplace(node);
    return false;
}

void Timer::adjust(const TimerNode &node)
{
    itimerspec new_val;
    memset(&new_val, 0, sizeof new_val);
    NanoSeconds duration = node.expires_ - Clock::now();
    int64_t val = duration.count();
    timespec ts;
    ts.tv_sec = val / NanoSecondsPerSecond;
    ts.tv_nsec = val % NanoSecondsPerSecond;
    new_val.it_value = ts;
    timerfd_settime(channel_->fd(), 0, &new_val, NULL);
}