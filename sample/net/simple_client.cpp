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

void ConnectionCallbackFunction(const TcpConnectionPtr& connect) {
    if (connect->IsConnected()) {
        char buffer[] = "ssssssss";
        connect->Send(buffer, sizeof(buffer));
    }
}

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    EventLoop loop;
    TcpClient client(&loop, std::string("client"), address);
    client.SetConnectionCallback(std::bind(
        &ConnectionCallbackFunction,
        std::placeholders::_1));
    client.Connect();
    loop.Loop();
    return 0;    
}

