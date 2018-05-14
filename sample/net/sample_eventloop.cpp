#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>

#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace Net;

class TcpServer {
 public:
    TcpServer(EventLoop *loop, TcpIPv4Socket *listen_socket) :
        local_loop_(loop),
        listen_socket_(listen_socket) {}

    void start() {
        std::shared_ptr<Channel> channel(new Channel(local_loop_, listen_socket_->GetSocketFd()));
        channel->SetReadCallback(
            std::bind(&TcpServer::HandleNewConnection, this));
        channel->EnableReading(true);
        channel_.insert(
            std::make_pair(listen_socket_->GetSocketFd(), channel));
    }


    void HandleNewConnection() {
        int socket_fd = -1;
        Status status = listen_socket_->Accept(&socket_fd);
        if (SOCKET_ACCEPT_FAILED == status) {
            LOG_WARN << "accept a bad socket fd.";
            return;
        } else if (SUCCESS == status) {
        } else {
            LOG_FATAL << "accept occored a error.";
        }
        std::shared_ptr<Channel> channel(new Channel(local_loop_, socket_fd));
        std::shared_ptr<TcpIPv4Socket> socket(new TcpIPv4Socket(socket_fd));
        channel_.insert(
            std::make_pair(channel->GetFd(), channel));
        socket_.insert(
            std::make_pair(socket->GetSocketFd(), socket));
        channel->SetReadCallback(std::bind(&TcpServer::handleRead, this, channel->GetFd()));
        channel->EnableReading(true);
    }

    void handleRead(int index) {
        char buf[1000];
        socket_[index]->Read(buf, sizeof(buf));
        std::cout << buf << std::endl;
    }

 private:
    typedef std::map<int, std::shared_ptr<Channel>> ChannelMap_;
    typedef std::map<int, std::shared_ptr<TcpIPv4Socket>> SocketMap_;
    ChannelMap_ channel_;
    SocketMap_ socket_;
    EventLoop *local_loop_;
    TcpIPv4Socket *listen_socket_;
};

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    TcpIPv4Socket socket;
    Status status = socket.BindAddress(address);
    if (SUCCESS != status) {
        LOG_ERROR << "socket bind address failed.";
        return -1;
    }
    status = socket.Listen();
    if (SUCCESS != status) {
        LOG_ERROR << "socket listen failed.";
        return -1;
    }
    EventLoop loop;
    TcpServer server(&loop, &socket);
    server.start();
    loop.Loop();
    return 0;
}

