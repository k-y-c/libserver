#pragma once
#include <sys/epoll.h>
#include <vector>
#include <set>
#include "Channel.hpp"

class Channel;

class Epoller{
public:
    Epoller(int maxEvent = 16):epoll_fd(epoll_create(1)),events_(maxEvent){

    }
    ~Epoller(){
         // TODO 关闭epoll
    }

    void update(Channel* channel,uint32_t events);

    void remove(Channel* channel);
    
    int wait(int timeoutMs);

    Channel* getEventChannel(int i);

    uint32_t getEvents(int i);

private:
    bool addChannel(Channel* channel,uint32_t events);

    bool modChannel(Channel* channel, uint32_t events);

    bool delChannel(Channel* channel);
    
private:
    int epoll_fd;
    std::set<Channel*> current_channels;
    std::vector<epoll_event> events_;
};