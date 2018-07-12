#include <stdio.h>

#include <TcpClient.hpp>
#include <Logging.hpp>
#include <Connector.hpp>
#include <EventLoop.hpp>
#include <Socket.hpp>

namespace Net {

void RemoveConnect(EventLoop* loop, const TcpConnectionPtr& connection){
    loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, connection));
}

void RemoveConnector(const ConnectorPtr& connector) {}


TcpClient::TcpClient(
    EventLoop* loop,
    const std::string name,
    const InetAddressIPV4 server) : 
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
        this,
        std::placeholders::_1));
    LOG_INFO << "Create TcpClient : " << name_;
}

TcpClient::~TcpClient() {
    LOG_INFO << "Destory TcpClient : " << name_;
    TcpConnectionPtr connection;
    bool unique = false;
    {
        Threads::MutexLockGuard lock(mutex_);
        unique = connection_.unique();
        connection = connection_;
    }
    if (connection) {
        CloseCallback callback = std::bind(
            &RemoveConnect,
            local_loop_,
            std::placeholders::_1);
        local_loop_->RunInLoop(std::bind(
            &TcpConnection::SetCloseCallback,
            connection,
            callback));
        if (unique) {
            connection->ForceClose();
        }
    } else {
        connector_->Stop();
    }
}

void TcpClient::Connect() {
    LOG_INFO << "TcpClient : " << name_ << " connecting to "
        << connector_->ServerAddress().GetIPAndPort();
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

TcpConnectionPtr TcpClient::Connection()  {
    Threads::MutexLockGuard lock(mutex_);
    return connection_;
}

EventLoop* TcpClient::GetLoop() const {
    return local_loop_;    
}

bool TcpClient::Retry() const {
    return retry_;    
}
    
void TcpClient::EnableRetry() {
    retry_ = true;    
}

const std::string TcpClient::GetName() {
    return name_;    
}

void TcpClient::SetConnectionCallback(ConnectionCallback callback) {
    connection_callback_ = std::move(callback);    
}

void TcpClient::SetMessageCallback(MessageCallback callback) {
    message_callback_ = std::move(callback);    
}

void TcpClient::SetWriteCompleteCallback(WriteCompleteCallback callback) {
    write_complete_callback_ = std::move(callback);
}

void TcpClient::NewConnection(int sockfd) {
    if (!local_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";    
    }
    
    TcpIPv4Socket socket(sockfd);

    InetAddressIPV4 peer_address(0, "127.0.0.1");
    socket.GetPeerAdress(&peer_address);

    InetAddressIPV4 local_address(0, "127.0.0.1");
    socket.GetLocalAdress(&local_address);

    char buffer[60];
    snprintf(
        buffer,
        sizeof(buffer),
        ":%s#%d",
        peer_address.GetIPAndPort().c_str(),
        next_connect_id_);
    ++next_connect_id_;
    std::string connect_name = name_ + buffer;

    TcpConnectionPtr connect(new TcpConnection(
        local_loop_,
        connect_name,
        sockfd));

    connect->SetConnectionCallback(connection_callback_);
    connect->SetMessageCallback(message_callback_);
    connect->SetWriteCompleteCallback(write_complete_callback_);
    connect->SetCloseCallback(std::bind(
        &TcpClient::RemoveConnection,
        this,
        std::placeholders::_1));
    {
        Threads::MutexLockGuard lock(mutex_);
        connection_ = connect;
    }
    connect->ConnectEstablished();
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
    if (retry_ && connect_) {
        connector_->Restart();
    }
}

}
