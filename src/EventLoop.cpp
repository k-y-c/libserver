#include "EventLoop.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include "Thread.hpp"

EventLoop::EventLoop():
poller_(new Epoller()),
wakeupFd_(eventfd(0,EFD_CLOEXEC|EFD_NONBLOCK)),
wakeupChannel_(new Channel(this,wakeupFd_)),
lock(new Locker),
threadId_(tid())
{
    wakeupChannel_->enableRead();
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead,this));
};

void EventLoop::handleRead(){
    uint64_t data = 0;
    ::read(wakeupFd_,&data,sizeof data);
    LOG_INFO << "EventLoop wakeuped!";
}

void EventLoop::wakeup(){
    uint64_t data = 1;
    ::write(wakeupFd_,&data,sizeof data);
}

void EventLoop::loop()
{

    while (true)
    {
        int num_events = poller_->wait(0);
        for (int i = 0; i < num_events; ++i)
        {
            Channel *ch = poller_->getEventChannel(i);
            ch->handleEvent();
        }
        doFunctors();
    }
}

void EventLoop::doFunctors(){
        std::vector<Functor> functors_;
        {
            LockerGuard guard(*lock);
            functors_.swap(functors);
        }
        for(int i = 0;i<functors_.size();++i){
            functors_[i]();
        }
}

void EventLoop::removeChannel(Channel* channel){
    poller_->remove(channel);
}

void EventLoop::runInLoop(const Functor& cb){
    if(isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb){
    {
        LockerGuard guard(*lock);
        functors.push_back(cb);
    }
    // TODO 判断是否需要wakeup
    wakeup();
}

bool EventLoop::isInLoopThread(){
    return threadId_ == tid();
}