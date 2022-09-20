#include "TcpServer.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "Socket.hpp"
#include "TcpConnection.hpp"
#include <functional>
#include "Thread.hpp"
#include <string>

class MyServer{
    public:
        MyServer():loop(),addr(9999),server(&loop,addr),pool(4){
            server.setConnectionCallback(std::bind(&MyServer::onConnection,this,std::placeholders::_1));
            server.setMessageCallback(std::bind(&MyServer::onMessage,this,std::placeholders::_1,std::placeholders::_2));
            server.setWriteCompleteCallback(std::bind(&MyServer::onWriteComplete,this,std::placeholders::_1));
            std::string line;
            
            for (int i = 33; i < 127; ++i)
            {
                line.push_back(char(i));
            }
            line += line;

            for (size_t i = 0; i < 127-33; ++i)
            {
                message += line.substr(i, 72) + '\n';
            }
            for(int i = 0;i<10;++i){
                message+=message;
            }
        }

        void onConnection(TcpConnectionPtr conn){
            LOG_INFO << "new connection";
            conn->send(message);
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

        void onWriteComplete(TcpConnectionPtr conn){
            conn->send(message);
        }

        void onMessage(TcpConnectionPtr conn,Buffer* buf){
            LOG_INFO << "onMessage";
            // pool.run(std::bind(&MyServer::calculate,this,conn,buf));
            // std::string recv = buf->retrieveAllAsString();
            // LOG_INFO << "receive: " << recv.c_str();
            // conn->send(recv);
            // conn->shutDown();


        }

        void testRunInLoop(){
            LOG_INFO << "testRunInLoop";
        }

        void start(){
            pool.start();
            server.start();
        }

    private:
        ThreadPool pool;
        EventLoop loop;
        InetAddress addr;
        TcpServer server;
        std::string message;
};

int main(){
    nanolog::initialize(nanolog::GuaranteedLogger(), ".", "nanolog", 1);
    MyServer server;
    server.start();
    return 0;
}