#pragma once
#include <pthread.h>
#include "Log.hpp"

class Locker{
public:
    friend class Cond;

    Locker(){
        pthread_mutex_init(&mutex,NULL);
    }

    ~Locker(){
        pthread_mutex_destroy(&mutex);
    };

    void lock(){
        pthread_mutex_lock(&mutex);
    }

    void unlock(){
        pthread_mutex_unlock(&mutex);
    }
private:
    pthread_mutex_t mutex;
};

class LockerGuard{
public:
    LockerGuard(Locker& locker):locker_(locker){
        locker_.lock();
    }
    ~LockerGuard(){
        locker_.unlock();
    }

private:
    Locker locker_;
};

class Cond{
public:
    Cond(){
        pthread_cond_init(&cond,NULL);
    }

    ~Cond(){
        pthread_cond_destroy(&cond);
    }

    void wait(Locker& locker){
        pthread_cond_wait(&cond,&locker.mutex);
    }

    void notify_once(){
        pthread_cond_signal(&cond);
    }

    void notify_all(){
        pthread_cond_broadcast(&cond);
    }

private:
    // pthread_mutex_t mutex;
    pthread_cond_t cond;
};