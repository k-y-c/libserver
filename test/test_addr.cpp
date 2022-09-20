#include "Socket.hpp"
#include <iostream>
using namespace std;
int main(){
    InetAddress ad("127.0.0.1",9090);
    auto addr = ad.getSockaddr();
    cout << addr->sa_data<< endl;
    return 0;
}