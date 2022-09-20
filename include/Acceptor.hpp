#pragma once

#include "EventLoop.hpp"
#include "Channel.hpp"
#include "Socket.hpp"
#include <functional>
#include <unistd.h>
#include "InetAddress.hpp"

class Acceptor
{

public:
    using NewConnectionCallback = std::function<void(int,InetAddress&)>;
    Acceptor(EventLoop* loop,InetAddress& listenAddr):
        loop_(loop),

        acceptChannel_(loop,initListenSockFd(listenAddr))
    {
        acceptChannel_.setReadCallback(std::bind(&Acceptor::OnConnection,this));
    };
    ~Acceptor(){};

    void setConnectionCallback(NewConnectionCallback cb){
        newConnectionCallback = cb;
    }

    void OnConnection(){
        // LOG_INFO << "OnConnection";
        InetAddress peerAddr;
        socklen_t len = sizeof peerAddr;
        int connfd = accept4(listenFd_,(sockaddr*)&peerAddr,&len,SOCK_NONBLOCK|SOCK_CLOEXEC);
        LOG_INFO << "accept connfd: " << connfd;
        if(newConnectionCallback){
            // LOG_INFO << "newConnectionCallback";
            newConnectionCallback(connfd,peerAddr);
        }
        else{
            LOG_INFO << "close";
            close(connfd);
        }
    }

    void listen(){
        acceptChannel_.enableRead();
        ::listen(listenFd_,1024);
    }

    int initListenSockFd(InetAddress& addr){
        listenFd_ = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
        int optval = 1;
        setsockopt(listenFd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
        setsockopt(listenFd_,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof optval); // 端口复用，避免服务器重启等待
        bind(listenFd_,addr.getSockaddr(),sizeof addr);
        return listenFd_;
    }

private:
    EventLoop* loop_;
    Channel acceptChannel_;
    int listenFd_;
    NewConnectionCallback newConnectionCallback;
};
