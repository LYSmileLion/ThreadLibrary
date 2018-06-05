#include <Connector.hpp>
#include <Logging.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>
#include <Socket.hpp>

namespace Net {

Connector::Connector(EventLoop* loop, const InetAddressIPV4& server) : 
    local_loop_(loop),
    server_addr_(server),
    connect_(false),
    status_(DISCONNECTED),
    retry_delay_ms_(1000) {}

Connector::~Connector() {}

void Connector::Start() {
    connect_ = true;
    local_loop_->RunInLoop(
        std::bind(&Connector::StartInLoop, shared_from_this()));
}

void Connector::StartInLoop() {
    if (!local_loop_->IsInLoopThread()) {
        LOG_FATAL << "loop not in loop thread.";
    }
    if (DISCONNECTED != status_) {
        LOG_FATAL << "status is error.";
    }

    if (connect_) {
        Connect();
    } else {
        LOG_DEBUG << "do not connect";
    }
}

void Connector::Stop() {
    connect_ = false;
    local_loop_->QueueInLoop(std::bind(&Connector::StopInLoop, shared_from_this()));
}

void Connector::StopInLoop() {
    if (!local_loop_->IsInLoopThread()) {
        LOG_FATAL << "loop not in loop thread.";
    }
    if (CONNECTING == status_) {
        SetState(DISCONNECTED);
        int sockfd = RemoveAndResetChannel();
        Retry(sockfd);
    }
}

void Connector::Connect() {
    TcpIPv4Socket socket;
    int sockfd = socket.GetFd();
    sockaddr_in address = server_addr_.GetSocketAddr();
    int ret = ::connect(
        sockfd,
        reinterpret_cast<sockaddr *>(&address),
        static_cast<socklen_t>(sizeof(address)));
    int savedErrno = ((ret == 0) ? 0 : errno);
    switch (savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            Connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case ECONNREFUSED:
        case ENETUNREACH:
            Retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
            socket.Close();
            break;

        default:
            LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
            socket.Close();
            break;
    }
}

void Connector::Restart() {
    if (!local_loop_->IsInLoopThread()) {
        LOG_FATAL << "loop not in loop thread.";
    }
    SetState(DISCONNECTED);
    retry_delay_ms_ = 1000;
    connect_ = true;
    StartInLoop();
}

void Connector::Connecting(int sockfd) {
    SetState(CONNECTING);
    channel_.reset(new Channel(local_loop_, sockfd));
    channel_->SetWriteCallback(
        std::bind(&Connector::HandleWrite, shared_from_this()));
    channel_->SetErrorCallback(
            std::bind(&Connector::HandleError, shared_from_this()));

    channel_->EnableWriting(true);
}

int Connector::RemoveAndResetChannel() {
    channel_->DisableAll();
    channel_->RemoveSelfInPoll();
    int sockfd = channel_->GetFd();
    // Can't reset channel_ here, because we are inside Channel::handleEvent
    local_loop_->QueueInLoop(
        std::bind(&Connector::ResetChannel, shared_from_this()));
    return sockfd;
}

void Connector::ResetChannel() {
    channel_.reset();
}

void Connector::HandleWrite() {
    LOG_INFO << "Connector::handleWrite " << status_;

    if (CONNECTING == status_) {
        int sockfd = RemoveAndResetChannel();
        TcpIPv4Socket socket(sockfd);
        int err = 0;
        socket.GetErrorCode(&err);
        if (err) {
            LOG_WARN << "Connector::handleWrite - SO_ERROR = ";
            Retry(sockfd);
        //} else if (sockfd.IsSelfConnect(sockfd)) {
        //    LOG_WARN << "Connector::handleWrite - Self connect";
        //    Retry(sockfd);
        } else {
            SetState(CONNECTED);
            if (connect_) {
                connection_callback_(sockfd);
            } else {
                socket.Close();
            }
        }
    } else {
        LOG_FATAL << "occered some unknow error.";
    }
}

void Connector::HandleError() {
    LOG_ERROR << "Connector::handleError state=" << status_;
    if (CONNECTING == status_) {
        int sockfd = RemoveAndResetChannel();
        TcpIPv4Socket socket(sockfd);
        int err = 0;
        socket.GetErrorCode(&err);
        LOG_TRACE << "SO_ERROR = " << err << " ";
        Retry(sockfd);
    }
}

void Connector::Retry(int sockfd) {
    TcpIPv4Socket socket(sockfd);
    socket.Close();
    SetState(DISCONNECTED);
    if (connect_) {
        LOG_INFO << "Connector::retry - Retry connecting to " <<
            server_addr_.GetIPAndPort() << " in ";
        local_loop_->QueueInLoop(
            std::bind(&Connector::StartInLoop, shared_from_this()));
    } else {
        LOG_DEBUG << "do not connect";
    }
}

}
