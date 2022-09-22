#pragma once

#include <sys/timerfd.h>
#include <chrono>
#include <functional>
#include <set>
#include <map>
#include "EventLoop.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <assert.h>
#include "Callback.hpp"
#include <atomic>
#include "Log.hpp"

using Clock = std::chrono::high_resolution_clock;
using TimeStamp = Clock::time_point;
using Seconds = std::chrono::seconds;
using MilliSeconds = std::chrono::milliseconds;
using NanoSeconds = std::chrono::nanoseconds;
const int64_t NanoSecondsPerSecond = 1000 * 1000 * 1000;

class EventLoop;
class Channel;

struct TimerNode
{
    TimerNode(unsigned int id, TimerCallback cb = NULL, TimeStamp expire = Clock::now(), MilliSeconds interval = MilliSeconds(0))
        : id_(id),
          cb_(cb),
          expires_(expire),
          interval_(interval)
    {
    }
    ~TimerNode(){
        // LOG_INFO << "TimerNode " << id_ << " deleted";
    }
    unsigned int id_;
    TimerCallback cb_;
    TimeStamp expires_;
    MilliSeconds interval_;
};

bool operator<(const TimerNode &lhs, const TimerNode &rhs);

class Timer
{
public:
    using TimerNodePtr = std::shared_ptr<TimerNode>;

    Timer(EventLoop *loop);

    unsigned int addTimer(TimerCallback cb, TimeStamp ts, MilliSeconds interval);

    void cancel(unsigned int id);

private:
    unsigned int getNewId()
    {
        unsigned int id = id_.fetch_add(1);
        while (timerIdx_.count(id) != 0)
        {
            id = id_.fetch_add(1);
        }
        timerIdx_[id] = nullptr;
        return id;
    }
    void handleRead();
    void addTimerInLoop(unsigned int id, TimerNodePtr node);
    void cancelInLoop(unsigned int id);
    bool insert(TimerNodePtr node);
    void adjust(TimerNodePtr node);

    static bool Comp_(const TimerNodePtr lhs, const TimerNodePtr rhs)
    {
        return *lhs < *rhs;
    }

private:
    EventLoop *loop_;
    std::set<TimerNodePtr, decltype(&Comp_)> timers_;

    std::map<int, TimerNodePtr> timerIdx_;
    Channel *channel_;
    std::atomic<unsigned int> id_;
};