#include <stdio.h>

#include <TcpClient.hpp>
#include <Logging.hpp>
#include <Connector.hpp>
#include <EventLoop.hpp>
#include <Socket.hpp>

namespace Net {

void DefaultConnectionCallback(const TcpConnectionPtr& connection) {
    LOG_INFO << connection->GetLocalAddress().GetIPAndPort() << " -> "
        << connection->GetPeerAddress().GetIPAndPort() << " is "
        << (connection->IsConnected() ? "UP" : "DOWN");
}

void DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf) { 
    LOG_INFO << buf->RetrieveAllAsString();
}

void RemoveConnection(EventLoop* loop, const TcpConnectionPtr& connection){
    loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, connection));
}

void RemoveConnector(const ConnectorPtr& connector) {}


TcpClient::TcpClient(
    EventLoop* loop,
    const InetAddressIPV4 server,
    const std::string name) :
        local_loop_(loop),
        connector_(new Connector(loop, server)),
        name_(name),
        connection_callback_(DefaultConnectionCallback),
        message_callback_(DefaultMessageCallback),
        retry_(false),
        connect_(true),
        next_connect_id_(1) {
    connector_->SetNewConnectionCallback(std::bind(
        &TcpClient::NewConnection,
        shared_from_this(),
        std::placeholders::_1));

    LOG_INFO << "TcpClient::TcpClient[" << name_
        << "] - connector ";
}

TcpClient::~TcpClient() {
    LOG_INFO << "TcpClient::~TcpClient[" << name_
        << "] - connector ";
    TcpConnectionPtr conn;
    bool unique = false;
    {
        Threads::MutexLockGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn) {
        assert(loop_ == conn->getLoop());
        // FIXME: not 100% safe, if we are in different thread
        CloseCallback cb = std::bind(
            &RemoveConnection,
            local_loop_,
            std::placeholders::_1);
        local_loop_->RunInLoop(std::bind(
            &TcpConnection::SetCloseCallback,
            conn,
            cb));
        if (unique) {
            conn->ForceClose();
        }
    } else {
        connector_->Stop();
        // FIXME: HACK
        //aloop_->runAfter(1, std::bind(&RemoveConnector, connector_));
    }
}

void TcpClient::Connect() {
    //LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to "
    //    << connector_->serverAddress().toIpPort();
    connect_ = true;
    connector_->Start();
}

void TcpClient::Disconnect() {
    connect_ = false;
    {
        Threads::MutexLockGuard lock(mutex_);
        if (connection_) {
            connection_->Shutdown();
        }
    }
}

void TcpClient::Stop() {
    connect_ = false;
    connector_->Stop();
}

void TcpClient::NewConnection(int sockfd) {
    if (local_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";    
    }
    
    TcpIPv4Socket socket(sockfd);

    InetAddressIPV4 peer_address(0, "127.0.0.1");
    socket.GetPeerAdress(&peer_address);

    InetAddressIPV4 local_address(0, "127.0.0.1");
    socket.GetLocalAdress(&local_address);

    char buffer[32];
    snprintf(
        buffer,
        sizeof(buffer),
        ":%s#%d",
        peer_address.GetIPAndPort().c_str(),
        next_connect_id_);
    ++next_connect_id_;
    std::string connect_name = name_ + buffer;

    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(
        local_loop_,
        connect_name,
        sockfd,
        local_address,
        peer_address));

    conn->SetConnectionCallback(connection_callback_);
    conn->SetMessageCallback(message_callback_);
    conn->SetWriteCompleteCallback(write_complete_callback_);
    conn->SetCloseCallback(std::bind(
        &TcpClient::RemoveConnection,
        shared_from_this(),
        std::placeholders::_1));
    {
        Threads::MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->ConnectEstablished();
}

void TcpClient::RemoveConnection(const TcpConnectionPtr& conn) {
    if (local_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";    
    }
    assert(loop_ == conn->GetLoop());

    {
        Threads::MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    local_loop_->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
    if (retry_ && connect_)
    {
        //LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
        //    << connector_->serverAddress().toIpPort();
        connector_->Restart();
    }
}

}
