#pragma once
#include "Epoller.hpp"
#include "Channel.hpp"
#include <memory>
#include <functional>
#include <queue>
#include "Mutex.hpp"

class Epoller;
class Channel;

class EventLoop
{
public:
    friend class Channel;
    using Functor = std::function<void()>;
    EventLoop(/* args */);
    ~EventLoop(){};

    void loop();

    void handleRead();

    void wakeup();

    void runInLoop(const Functor& cb);

    void queueInLoop(const Functor& cb);

    void doFunctors();

    void removeChannel(Channel* channel);

    bool isInLoopThread();

private:
    // Epoller* poller_; 
    std::shared_ptr<Epoller> poller_;
    int wakeupFd_;
    std::shared_ptr<Channel> wakeupChannel_;
    std::vector<Functor> functors;
    Locker* lock;
    const pid_t threadId_;
};

