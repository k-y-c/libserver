#pragma once
#include "EventLoop.hpp"
#include "EventLoopThreadPool.hpp"
#include "Acceptor.hpp"
#include "TcpConnection.hpp"
#include <functional>
#include <map>
#include "Callback.hpp"
#include "InetAddress.hpp"

class TcpServer
{

public:

    TcpServer(EventLoop* loop,InetAddress& listenAddr):
    baseLoop_(loop),
    acceptor_(new Acceptor(loop,listenAddr)),
    localAddr_(listenAddr),
    eventLoopPool_(new EventLoopThreadPool(loop))
    {
        acceptor_->setConnectionCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));
    };
    ~TcpServer(){};

    void setConnectionCallback(ConnectionCallback cb){connectionCallback = cb;}
    void setMessageCallback(MessageCallback cb){messageCallback = cb;}
    void setCloseCallback(CloseCallback cb){closeCallback = cb;}
    void setWriteCompleteCallback(WriteCompleteCallback cb){writeCompleteCallback = cb;};

    void start(){
        LOG_INFO << "Server Start!";
        baseLoop_->runInLoop(std::bind(&Acceptor::listen,acceptor_));
        eventLoopPool_->start();
        baseLoop_->loop();
    }

    void setThreadNum(int num){
        eventLoopPool_->setThreadNum(num);
    }

private:
    void newConnection(int fd,InetAddress& peerAddr){
        LOG_INFO << "new connection: " << peerAddr.toIpPort().c_str();
        EventLoop* ioLoop = eventLoopPool_->getNextLoop();
        TcpConnectionPtr conn(new TcpConnection(ioLoop,fd,localAddr_,peerAddr));
        connections_[fd] = conn;
        conn->setReadCallback(messageCallback);
        conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));
        conn->setConnectionCallback(connectionCallback);
        conn->setWriteCompleteCallback(writeCompleteCallback);
        ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished,conn));
        // conn->connectionEstablished();
    }

    void removeConnection(TcpConnectionPtr tcpConn){
        LOG_INFO << "removeConnection";
        baseLoop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,tcpConn));        
    }

    void removeConnectionInLoop(TcpConnectionPtr tcpConn){
        LOG_INFO << "removeConnectionInLoop";
        connections_.erase(tcpConn->id());
        
        // FIXME multiloop时需要修改
        EventLoop* ioLoop = tcpConn->getLoop();
        // tcpConn->connectionDetroyed();
        ioLoop->runInLoop(std::bind(&TcpConnection::connectionDetroyed,tcpConn));
    }

private:
    EventLoop* baseLoop_;
    EventLoopThreadPool* eventLoopPool_;
    Acceptor* acceptor_;
    ConnectionCallback connectionCallback;
    MessageCallback messageCallback;
    CloseCallback closeCallback;
    WriteCompleteCallback writeCompleteCallback;
    InetAddress localAddr_;
    std::map<int,TcpConnectionPtr> connections_;
};

