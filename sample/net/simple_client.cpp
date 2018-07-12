#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>
#include <TcpClient.hpp>
#include <EventLoop.hpp>

#include <string>
#include <iostream>

#include<unistd.h>

using namespace Net;

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    EventLoop loop;
    TcpClient client(&loop, std::string("client"), address);
    client.Connect();
    loop.Loop();
    return 0;    
}

