#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>
#include <Acceptor.hpp>
#include <TcpConnection.hpp>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <functional>

using namespace Net;
using namespace std::placeholders;

class TcpServer {
 public:
    TcpServer(EventLoop *loop, const InetAddressIPV4 address) :
        acceptor_(loop, address),
        local_loop_(loop) {}



    void HandleNewConnection(int sockfd) {
        std::shared_ptr<TcpConnection> connect(
            new TcpConnection(local_loop_, "connection", sockfd));
        connect_.insert(
            std::make_pair(sockfd, connect));

        std::cout << "ssssssssssssssssssss" << std::endl;
        connect->SetCloseCallback(
            std::bind(&TcpServer::DefaultCloseBack, this, connect));
        connect->ConnectEstablished();
    }
    
    void start() {
        acceptor_.SetNewConnectionCallback(
            std::bind(&TcpServer::HandleNewConnection, this, _1));
        acceptor_.Start();
    }

 private:
    void DefaultCloseBack(const TcpConnectionPtr&) {
        
    }
 private:
    typedef std::map<int, std::shared_ptr<TcpConnection>> ConnectMap_;
    ConnectMap_ connect_;
    Acceptor acceptor_;
    EventLoop *local_loop_;
};

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    EventLoop loop;
    TcpServer server(&loop, address);
    server.start();
    loop.Loop();
    return 0;
}

