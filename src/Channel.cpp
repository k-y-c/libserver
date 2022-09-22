#include "Channel.hpp"
#include "EventLoop.hpp"

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_(0){

                                                                  };

Channel::~Channel() { LOG_INFO << "channel " << fd_ << " removed"; }

void Channel::enableRead()
{
    events_ |= kReadEvent;
    loop_->poller_->update(this, events_);
}

void Channel::disableRead()
{
    events_ ^= kReadEvent;
    loop_->poller_->update(this, events_);
}

void Channel::enableWrite()
{
    events_ |= kWriteEvent;
    loop_->poller_->update(this, events_);
}

void Channel::disableWrite()
{
    events_ ^= kWriteEvent;
    loop_->poller_->update(this, events_);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

void Channel::disableAll()
{
    events_ = 0;
    loop_->poller_->update(this, events_);
}

void Channel::handleEvent()
{
    if (revents_ & EPOLLHUP)
    {
        // 关闭连接
        LOG_INFO << "closeCallback";
        if (closeCallback)
            closeCallback();
    }
    else if (revents_ & EPOLLIN)
    {
        // 有消息到来
        // LOG_INFO << "readCallback";
        if (readCallback)
            readCallback();
    }
    else if (revents_ & EPOLLOUT)
    {
        // 写事件
        // handleWrite(fd);
        if (writeCallback)
            writeCallback();
    }
    else if (revents_ & EPOLLERR)
    {
        LOG_INFO << "ERROR events";
    }
    else
    {
        LOG_INFO << "unkown events";
    }
}