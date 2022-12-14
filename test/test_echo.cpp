#include "TcpServer.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "Socket.hpp"
#include "TcpConnection.hpp"
#include <functional>
#include "Thread.hpp"
#include <string>

class MyServer
{
public:
    MyServer() : loop(), addr(9999), server(&loop, addr), pool(4)
    {
        server.setConnectionCallback(std::bind(&MyServer::onConnection, this, std::placeholders::_1));
        server.setMessageCallback(std::bind(&MyServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
        // server.setWriteCompleteCallback(std::bind(&MyServer::onWriteComplete,this,std::placeholders::_1));
        server.setCloseCallback(std::bind(&MyServer::onClose, this, std::placeholders::_1));
        // server.setThreadNum(2);
    }

    void onConnection(TcpConnectionPtr conn)
    {
        LOG_INFO << "new connection";
        EventLoop *loop = conn->getLoop();
        nodeid = loop->runEvery(2000, std::bind(&MyServer::pulse, this, conn));
        // conn->send(message);
    }

    // void calculate(TcpConnectionPtr conn,Buffer* buf){
    //     // std::string recv = buf->retrieveAllAsString();
    //     const void* data = buf->peek();
    //     size_t len = buf->readable();
    //     buf->retrieveAll();
    //     LOG_INFO << "receive bytes: " << len;

    //     conn->send(message);

    //     // conn->shutDown();
    //     // loop.runInLoop(std::bind(&MyServer::testRunInLoop,this));
    // }

    void onWriteComplete(TcpConnectionPtr conn)
    {
        // conn->send(message);
    }

    void onMessage(TcpConnectionPtr conn, Buffer *buf)
    {
        LOG_INFO << "onMessage";
        // pool.run(std::bind(&MyServer::calculate,this,conn,buf));
        const void *data = buf->peek();
        size_t len = buf->readable();
        buf->retrieveAll();
        LOG_INFO << "receive bytes: " << len;

        conn->send(data, len);
        // conn->shutDown();
    }

    void onClose(TcpConnectionPtr conn)
    {
        LOG_INFO << "onClose";
        EventLoop *loop = conn->getLoop();
        loop->cancel(nodeid);
    }

    void pulse(TcpConnectionPtr conn)
    {
        conn->send("this is a pulse message!\n");
    }

    void testRunInLoop()
    {
        LOG_INFO << "testRunInLoop";
    }

    void start()
    {
        pool.start();
        server.start();
    }

private:
    unsigned int nodeid;
    ThreadPool pool;
    EventLoop loop;
    InetAddress addr;
    TcpServer server;
    // std::string message;
};

int main()
{
    nanolog::initialize(nanolog::GuaranteedLogger(), ".", "nanolog", 1);
    MyServer server;
    server.start();
    return 0;
}