#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include "EventLoop.hpp"
#include <queue>
#include <vector>
#include <string>

class EventLoopThread
{
public:
    EventLoopThread(std::string name = "defalt loop")
        : name_(name),
          loop_(nullptr),
          mutex_(),
          cond_()
    {
    }

    EventLoop *startLoop()
    {
        thread_ = new std::thread(std::bind(&EventLoopThread::threadFunc, this));
        std::unique_lock<std::mutex> lck(mutex_);
        while (loop_ == nullptr)
        {
            cond_.wait(lck);
        }
        return loop_;
    }

    void threadFunc()
    {
        {
            std::lock_guard<std::mutex> guard(mutex_);
            loop_ = new EventLoop(name_);
        }
        cond_.notify_all();
        loop_->loop();
    }

private:
    EventLoop *loop_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread *thread_;
    std::string name_;
};

class EventLoopThreadPool
{

public:
    EventLoopThreadPool(EventLoop *loop)
        : baseloop_(loop),
          threadNum_(0){};
    ~EventLoopThreadPool();

    void setThreadNum(int n)
    {
        threadNum_ = n;
    }

    void start()
    {
        for (int i = 0; i < threadNum_; ++i)
        {
            std::string name = "ioLoop ";
            name += std::to_string(i);
            threads_.push_back(new EventLoopThread(name));
            pool_.push(threads_.back()->startLoop());
        }
    }

    void startLoop()
    {
    }

    EventLoop *getNextLoop()
    {
        if (threadNum_ == 0)
        {
            return baseloop_;
        }
        auto loop = pool_.front();
        pool_.pop();
        pool_.push(loop);
        return loop;
    }

private:
    EventLoop *baseloop_;
    int threadNum_;
    std::queue<EventLoop *> pool_;
    std::vector<EventLoopThread *> threads_;
};
