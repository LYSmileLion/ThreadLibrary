#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>
#include <Acceptor.hpp>
#include <TcpServer.hpp>
#include <TcpConnection.hpp>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <functional>

using namespace Net;
using namespace std::placeholders;

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    EventLoop loop;
    TcpServer server(&loop, "server", address);
    server.Start(5);
    loop.Loop();
    return 0;
}

