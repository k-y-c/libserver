#include "Socket.hpp"
#include "Server.hpp"
#include <iostream>
using namespace std;



int main(){
    nanolog::initialize(nanolog::GuaranteedLogger(), ".", "nanolog", 1);
    Server server;

    server.start();
    return 0;
}