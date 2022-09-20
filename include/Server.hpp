#pragma once
#include "Epoller.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Socket.hpp"
#include "Log.hpp"
#include "Buffer.hpp"
#include <string>
#include "TcpConn.hpp"
#include <map>
#include <bits/shared_ptr.h>

class Server
{
public:
    Server(int port = 9999):poller(){
        initListenSock(port);
    };
    ~Server(){};

    void start(){

        while(true){
            int num_events = poller.wait(1000);
            for(int i = 0;i<num_events;++i){
                int fd = poller.getEventFd(i);
                uint32_t events = poller.getEvents(i);
                if(fd == listenFd_){
                    //新连接
                    handleConnection();
                }
                else if(events & EPOLLRDHUP){
                    // 关闭连接
                    handleClose(fd);
                }
                else if(events & EPOLLIN){
                    // 有消息到来
                    handleRead(fd);
                }
                else if(events & EPOLLOUT){
                    // 写事件
                    // handleWrite(fd);
                }
                else{
                    LOG_INFO << "unkown events";
                }
            }
        }
    }
    
    void handleClose(int fd){
        LOG_INFO << "connection closed";
        poller.delFd(fd);
        close(fd);
    }

    void handleRead(int fd){
        Buffer buffer;
        int n = buffer.readFd(fd);
        if(n == 0){
            handleClose(fd);
            return;
        }
        LOG_INFO << "read data: " << buffer.retrieveAllAsString().c_str();
    }

    void handleConnection(){
        LOG_INFO << "new connection";
        sockaddr addr_;
        socklen_t len_;
        int fd = accept(listenFd_,&addr_,&len_);
        uint32_t event = EPOLLIN|EPOLLRDHUP;
        poller.addFd(fd,event);

    }

    void initListenSock(int port){
        listenFd_ = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
        int optval = 1;
        setsockopt(listenFd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
        setsockopt(listenFd_,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof optval); // 端口复用，避免服务器重启等待
        InetAddress addr(port);
        bind(listenFd_,addr.getSockaddr(),sizeof addr);
        listen(listenFd_,1024);
        uint32_t event = EPOLLIN|EPOLLRDHUP;
        poller.addFd(listenFd_,event);
    }


private:
    Epoller poller;
    int listenFd_;
    std::map<int,std::shared_ptr<TcpConn*>> tcpConns;
};
