#pragma once
// #include "Mutex.hpp"
#include <functional>
#include <pthread.h>
#include <queue>
#include "Log.hpp"
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include <mutex>
#include <condition_variable>

extern __thread pid_t currentTid;

void cacheTid();

pid_t tid();

// class Thread{
// public:
//     typedef std::function<void ()> ThreadFunc;
    
//     Thread(const ThreadFunc& func):func_(func){}
//     ~Thread(){}

//     void start(){
//         pthread_create(&thread_id,NULL,&Thread::run,this);
//     }
//     void join(){
//         pthread_join(thread_id,NULL);
//     }
//     void detach(){
//         pthread_detach(thread_id);
//     }
//     static void* run(void* item){
//         Thread* th = static_cast<Thread*>(item);
//         if(th->func_){
//             th->func_();
//         }
//         return NULL;
//     }

// private:
//     ThreadFunc func_;
//     pthread_t thread_id;
// };

class ThreadPool{
public:
    typedef std::function<void ()> Task;
    using LockerGuard = std::lock_guard<std::mutex>;
    ThreadPool(int nums):nums_(nums),locker(),cond(),stop_(false){

    };
    ~ThreadPool(){
        stop_ = true;
        cond.notify_all();
        stop();
    }

    void start(){
        for(int i = 0;i<nums_;++i){
            // Thread* th = new Thread(std::bind(&ThreadPool::threadFunc,this));
            // Threads.emplace_back(th);
            // th->start();
            auto th = new std::thread(std::bind(&ThreadPool::threadFunc,this));
            Threads.emplace_back(th);
        };
    }

    void stop(){
        for_each(Threads.begin(),Threads.end(),std::bind(&std::thread::join,std::placeholders::_1));
    }

    void threadFunc(){
        std::unique_lock<std::mutex> lck(this->locker);
        while(!stop_){
            Task task;      
            while(!stop_&&this->que.empty()){
                this->cond.wait(lck);
            }
            task = this->que.front();
            this->que.pop();
            lck.unlock();
            LOG_INFO << "run task";
            if(task)task();
            lck.lock();
        }
    }
    
    void run(Task task){
        {
            LockerGuard lock(locker);
            que.push(task);
        }
        cond.notify_one();
    }

private:
    int nums_;
    bool stop_;
    std::vector<std::thread*> Threads;
    std::queue<Task> que;
    // Locker locker;
    // Cond cond;
    std::mutex locker;
    std::condition_variable cond;
};