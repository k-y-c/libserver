#include "TcpServer.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "Socket.hpp"
#include "TcpConnection.hpp"
#include <functional>
#include "Thread.hpp"
#include <string>
#include <iostream>

using namespace std;


void print(){
    cout << "tick" << endl;
}
void printe(){
    cout << "tick every" << endl;
}


int main(){
    nanolog::initialize(nanolog::GuaranteedLogger(), ".", "nanolog", 1);
    EventLoop loop;
    auto node = loop.runAfter((1000),print);
    loop.cancel(node);
    loop.runAfter((2000),print);
    loop.runAfter((3000),print);
    loop.runEvery(1000,printe);
    loop.loop();
}