#pragma once
#include <endian.h>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "Socket.hpp"

class InetAddress
{
public:
  InetAddress(uint16_t port = 0)
  {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = hostToNetwork32(INADDR_ANY);
    addr_.sin_port = hostToNetwork16(port);
  }

  InetAddress(std::string ip, uint16_t port = 0)
  {
    memset(&addr_, 0, sizeof(addr_));
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
    addr_.sin_family = AF_INET;
    addr_.sin_port = hostToNetwork16(port);
  }

  sockaddr *getSockaddr()
  {
    return static_cast<sockaddr *>((void *)&addr_);
  }

  uint16_t getPort()
  {
    return networkToHost16(addr_.sin_port);
  }

  std::string toIpPort()
  {
    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,(void*)&addr_.sin_addr,buf,INET_ADDRSTRLEN);
    std::string res(buf);
    res += ":";
    res += std::to_string(getPort());
    return res;
  }

private:
  sockaddr_in addr_;
};
