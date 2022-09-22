
#include "TcpConnection.hpp"
#include "Callback.hpp"
#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Socket.hpp"
#include "Buffer.hpp"
#include <functional>
#include "Log.hpp"
// #include <boost/bind.hpp>

TcpConnection::TcpConnection(EventLoop *loop, int fd, InetAddress &local, InetAddress &peer)
    : loop_(loop),
      localAddr_(local),
      id_(fd),
      peerAddr_(peer),
      channel_(new Channel(loop, fd)),
      socket_(new Socket(fd))
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
};
TcpConnection::~TcpConnection()
{
    LOG_INFO << "TcpConnection destroyed : " << peerAddr_.toIpPort().c_str();
};

void TcpConnection::setReadCallback(MessageCallback cb) { messageCallback_ = cb; }
// void setWriteCallback(EventCallback cb){writeCallback_ = cb;}
void TcpConnection::setCloseCallback(CloseCallback cb) { closeCallback_ = cb; }

void TcpConnection::handleRead()
{
    int n = inputBuffer_.readFd(channel_->fd());
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &this->inputBuffer_);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {
        LOG_INFO << "read error!";
    }
}

void TcpConnection::handleWrite()
{
    ssize_t n = write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readable());
    if (n < 0)
        LOG_CRIT << "TcpConnection::handleWrite";
    outputBuffer_.retrieve(n);
    if (outputBuffer_.readable() == 0)
    {
        // LOG_INFO << "write down";
        channel_->disableWrite();
        if (writeCompleteCallback)
            writeCompleteCallback(shared_from_this());
    }
    else
    {
        LOG_INFO << "I'm going to write more data!";
    }
}

void TcpConnection::handleClose()
{
    // LOG_INFO << "handleClose";
    channel_->disableRead();
    channel_->disableWrite();
    closeCallback_(shared_from_this());
}

void TcpConnection::send(const std::string message)
{
    if (loop_->isInLoopThread())
    {
        sendInLoop(message);
    }
    else
    {
        // XXX
        // 使用std::bind无法识别重载函数，需要主动提供函数类型，而boost::bind则无此问题
        loop_->runInLoop(std::bind((void(TcpConnection::*)(const std::string &)) & TcpConnection::sendInLoop, this, message));
    }
}

void TcpConnection::send(const void *data, size_t len)
{
    if (loop_->isInLoopThread())
    {
        sendInLoop(data, len);
    }
    else
    {
        std::string message(static_cast<const char *>(data), len);
        loop_->runInLoop(std::bind((void(TcpConnection::*)(const std::string &)) & TcpConnection::sendInLoop, this, message));
    }
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
    // LOG_INFO << "sendInLoop data:" << static_cast<const char*>(data);
    ssize_t n = write(channel_->fd(), data, len);
    int remain = len - n;
    if (remain > 0)
    {
        LOG_INFO << "I'm going to write more data!";
        outputBuffer_.append(static_cast<const char *>(data) + n, remain);
        channel_->enableWrite();
    }
    else if (remain == 0)
    {
        // LOG_INFO << "write down";
        if (writeCompleteCallback)
        {
            writeCompleteCallback(shared_from_this());
        }
    }
    else
    {
        LOG_CRIT << "TcpConnection::sendInLoop";
    }
}

void TcpConnection::connectionEstablished()
{
    LOG_INFO << "connectionEstablished in loop: " << loop_->getLoopName().c_str();
    channel_->enableRead();
    if (connectionCallback_)
    {
        connectionCallback_(shared_from_this());
    }
}

void TcpConnection::connectionDetroyed()
{
    LOG_INFO << "connectionDetroyed";
    channel_->remove();
}
