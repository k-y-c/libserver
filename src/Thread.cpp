#include "Thread.hpp"
#include <unistd.h>
#include <sys/syscall.h>

__thread pid_t currentTid;

void cacheTid(){
    currentTid = ::syscall(SYS_gettid);
}

pid_t tid(){
    if(currentTid == 0){
        cacheTid();
    }
    return currentTid;
}