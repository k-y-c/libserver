#pragma once
#include "EventLoop.hpp"
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
    loop_(loop),
    acceptor_(new Acceptor(loop,listenAddr)),
    localAddr_(listenAddr)
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
        acceptor_->listen();
        loop_->loop();
    }


private:
    void newConnection(int fd,InetAddress& peerAddr){
        LOG_INFO << "new connection: " << peerAddr.toIpPort().c_str();
        TcpConnectionPtr conn(new TcpConnection(loop_,fd,localAddr_,peerAddr));
        connections_[fd] = conn;
        conn->setReadCallback(messageCallback);
        conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));
        conn->setConnectionCallback(connectionCallback);
        conn->setWriteCompleteCallback(writeCompleteCallback);
        conn->connectionEstablished();
    }

    void removeConnection(TcpConnectionPtr tcpConn){
        LOG_INFO << "removeConnection";
        loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,tcpConn));        
    }

    void removeConnectionInLoop(TcpConnectionPtr tcpConn){
        connections_.erase(tcpConn->id());
        
        // FIXME multiloop时需要修改
        tcpConn->connectionDetroyed();
        // loop_->runInLoop(std::bind(&TcpConnection::connectionDetroyed,tcpConn));
    }

private:
    EventLoop* loop_;
    Acceptor* acceptor_;
    ConnectionCallback connectionCallback;
    MessageCallback messageCallback;
    CloseCallback closeCallback;
    WriteCompleteCallback writeCompleteCallback;
    InetAddress localAddr_;
    std::map<int,TcpConnectionPtr> connections_;
};

