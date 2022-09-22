#pragma once
#include "EventLoop.hpp"
#include <functional>
#include "Log.hpp"
#include "Callback.hpp"
#include <sys/epoll.h>

class EventLoop;

class Channel
{

public:
    using EventCallback = std::function<void()>;
    friend class Epoller;
    Channel(EventLoop *loop, int fd);

    ~Channel();

    void enableRead();

    void disableRead();

    void enableWrite();

    void disableWrite();

    void disableAll();

    void remove();

    void handleEvent();

    void setReadCallback(EventCallback cb)
    {
        readCallback = cb;
    }

    void setWriteCallback(EventCallback cb)
    {
        writeCallback = cb;
    }

    void setCloseCallback(EventCallback cb)
    {
        closeCallback = cb;
    }

    int fd()
    {
        return fd_;
    }

private:
    EventLoop *loop_;
    int fd_;
    uint32_t events_;
    uint32_t revents_; // 返回的事件
    EventCallback readCallback;
    EventCallback writeCallback;
    EventCallback closeCallback;
    static const uint32_t kReadEvent = EPOLLIN | EPOLLPRI;
    static const uint32_t kWriteEvent = EPOLLOUT;
};
