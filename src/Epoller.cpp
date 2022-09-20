#include "Epoller.hpp"


void Epoller::update(Channel* channel,uint32_t events){
    if(current_channels.count(channel)){
        modChannel(channel,events);
    }
    else{
        current_channels.insert(channel);
        addChannel(channel,events);
    }
}

void Epoller::remove(Channel* channel){
    if(current_channels.count(channel)){
        delChannel(channel);
        current_channels.erase(channel);
    }
}

int Epoller::wait(int timeoutMs){
    return epoll_wait(epoll_fd,&events_[0],events_.size(),timeoutMs);
}

Channel* Epoller::getEventChannel(int i){
    Channel* ch = (Channel*)events_[i].data.ptr;
    ch->revents_ = getEvents(i);
    return ch;
}

uint32_t Epoller::getEvents(int i){
    return events_[i].events;
}

bool Epoller::addChannel(Channel* channel,uint32_t events){
    // if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.ptr = (void*)channel;
    ev.events = events;
    return 0 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, channel->fd_, &ev);
}

bool Epoller::modChannel(Channel* channel, uint32_t events) {
    // if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.ptr = (void*)channel;
    ev.events = events;
    return 0 == epoll_ctl(epoll_fd, EPOLL_CTL_MOD, channel->fd_, &ev);
}

bool Epoller::delChannel(Channel* channel) {
    // if(fd < 0) return false;
    epoll_event ev = {0};
    current_channels.erase(channel);
    return 0 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, channel->fd_, &ev);
}
