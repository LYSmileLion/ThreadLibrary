#include <TcpServer.hpp>
#include <Types.hpp>
#include <Logging.hpp>
#include <EventLoop.hpp>
#include <Acceptor.hpp>
#include <TcpConnection.hpp>

namespace Net {

TcpServer::TcpServer(EventLoop *loop, std::string name, const InetAddressIPV4 address) :
    acceptor_(loop, address),
    local_loop_(loop),
    next_connect_id_(1),
    started_(false),
    name_(name) {}


void TcpServer::SetConnectionCallback(const ConnectionCallback& cb) {
    connect_callback_ = cb;
}

void TcpServer::SetMessageCallback(const MessageCallback& cb) {
    message_callback_ = cb;
}

void TcpServer::SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    write_complete_callback_ = cb;
}

void TcpServer::HandleNewConnection(int sockfd) {
    char buffer[64];
    //TcpIPv4Socket socket(sockfd);
    //InetAddressIPV4 address;
    //Status status = socket.GetPeerAdress(&address);
    //if (SUCCESS != status) {
    //    LOG_ERROR << "GetPeerAddress failed.";
    //}
    snprintf(buffer, sizeof(buffer), "-%d", next_connect_id_);
    std::string name = name_ + buffer;
    std::shared_ptr<TcpConnection> connect(
        new TcpConnection(local_loop_, name, sockfd));
    connect_.insert(
        std::make_pair(name, connect));

    if (connect_callback_) {
        connect->SetConnectionCallback(connect_callback_);
    }
    if (message_callback_) {
        connect->SetWriteCompleteCallback(write_complete_callback_);
    }
    connect->SetCloseCallback(
        std::bind(&TcpServer::DefaultCloseBack, this, std::placeholders::_1));

    connect->ConnectEstablished();
}

void TcpServer::Start() {
        acceptor_.SetNewConnectionCallback(
            std::bind(&TcpServer::HandleNewConnection, this, std::placeholders::_1));
        acceptor_.Start();
}

void TcpServer::DefaultCloseBack(const TcpConnectionPtr& connect) {
    connect_.erase(connect->GetName());
}

}
