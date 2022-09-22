#pragma once

#include <sys/timerfd.h>
#include <chrono>
#include <functional>
#include <set>
#include "EventLoop.hpp"
#include "Channel.hpp"
#include <unistd.h>
#include <assert.h>
#include "Callback.hpp"

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
    TimerNode(TimerCallback cb, TimeStamp expire, MilliSeconds interval)
        : cb_(cb),
          expires_(expire),
          interval_(interval)
    {
    }
    int id;
    TimerCallback cb_;
    TimeStamp expires_;
    MilliSeconds interval_;
};

bool operator<(const TimerNode &lhs, const TimerNode &rhs);

class Timer
{
public:
    Timer(EventLoop *loop);

    TimerNode addTimer(TimerCallback cb, TimeStamp ts, MilliSeconds interval);

    void cancel(const TimerNode &node);

private:
    void handleRead();
    void addTimerInLoop(TimerCallback cb, TimeStamp ts, MilliSeconds interval);
    void cancelInLoop(const TimerNode node);
    bool insert(const TimerNode &node);
    void adjust(const TimerNode &node);

private:
    EventLoop *loop_;
    std::set<TimerNode> timers_;
    Channel *channel_;
};