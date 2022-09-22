#pragma once

#include "Callback.hpp"
#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "Buffer.hpp"
#include <functional>
#include "Log.hpp"
#include <string>
#include <unistd.h>
#include <memory>
#include "InetAddress.hpp"

class EventLoop;
class InetAddress;
class Channel;
class Buffer;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, int fd, InetAddress &local, InetAddress &peer);

    ~TcpConnection();

    void setConnectionCallback(ConnectionCallback cb) { connectionCallback_ = cb; };
    void setReadCallback(MessageCallback cb);
    // void setWriteCallback(EventCallback cb){writeCallback_ = cb;}
    void setCloseCallback(CloseCallback cb);
    void setWriteCompleteCallback(WriteCompleteCallback cb) { writeCompleteCallback = cb; }

    void handleRead();

    void handleWrite();

    void handleClose();

    void connectionEstablished();

    void connectionDetroyed();

    void send(const std::string message);

    void send(const void *data, size_t len);

    void shutDown()
    {
        state_ = Disconnecting;
        loop_->runInLoop(std::bind(&TcpConnection::shutDownInLoop, this));
    }

    void shutDownInLoop()
    {
        ::shutdown(channel_->fd(), SHUT_WR);
    }

    int id() { return id_; }

    EventLoop *getLoop()
    {
        return loop_;
    }

private:
    void sendInLoop(const std::string &message)
    {
        sendInLoop(message.c_str(), message.size());
    }

    void sendInLoop(const void *data, size_t len);

private:
    enum State
    {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting
    };

    EventLoop *loop_;
    // Channel* channel_;
    int id_;
    State state_;
    std::shared_ptr<Socket> socket_;
    std::shared_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    // EventCallback writeCallback_;

    WriteCompleteCallback writeCompleteCallback;
    CloseCallback closeCallback_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};
