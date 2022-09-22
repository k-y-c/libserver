#pragma once
#include <functional>
#include <memory>
// #include "TcpConnection.hpp"
// #include "Buffer.hpp"

class TcpConnection;
class Buffer;
class InetAddress;

// Channel
using EventCallback = std::function<void()>;

// Acceptor
using NewConnectionCallback = std::function<void(int,InetAddress&)>;

// Timer
using TimerCallback = std::function<void()>;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
//用户定义
using ConnectionCallback = std::function<void(TcpConnectionPtr)>;
using MessageCallback = std::function<void(TcpConnectionPtr,Buffer*)>;
using CloseCallback = std::function<void(TcpConnectionPtr)>;
using WriteCompleteCallback = std::function<void(TcpConnectionPtr)>;