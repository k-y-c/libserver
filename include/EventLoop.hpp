#pragma once

#include "Epoller.hpp"
#include "Channel.hpp"
#include <memory>
#include <functional>
#include <queue>
#include "Mutex.hpp"
#include <string>
#include "Callback.hpp"
#include "Timer.hpp"

class Epoller;
class Channel;
class Timer;
class TimerNode;

class EventLoop
{
public:
    friend class Channel;
    using Functor = std::function<void()>;
    EventLoop(std::string name = "default loop");
    ~EventLoop(){};

    void loop();

    TimerNode runAfter(uint64_t ms, TimerCallback cb);

    TimerNode runEvery(uint64_t ms, TimerCallback cb);

    void cancel(TimerNode &node);

    void handleRead();

    void wakeup();

    void runInLoop(const Functor &cb);

    void queueInLoop(const Functor &cb);

    void doFunctors();

    void removeChannel(Channel *channel);

    bool isInLoopThread();

    std::string getLoopName();

private:
    // Epoller* poller_;
    std::shared_ptr<Epoller> poller_;
    std::shared_ptr<Timer> timer_;
    int wakeupFd_;
    std::shared_ptr<Channel> wakeupChannel_;
    std::vector<Functor> functors;
    Locker *lock;
    const pid_t threadId_;
    std::string name_;
};
