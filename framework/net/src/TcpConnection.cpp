#include <errno.h>
#include <string.h>

#include <iostream>

#include <TcpConnection.hpp>
#include <Logging.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>
#include <Socket.hpp>
#include <InetAddress.hpp>

namespace Net {

void DefaultConnectionCallback(const TcpConnectionPtr& connection) {
    LOG_INFO << connection->GetLocalAddress().GetIPAndPort() << " -> "
        << connection->GetPeerAddress().GetIPAndPort() << " is "
        << (connection->IsConnected() ? "UP" : "DOWN");
}

void DefaultMessageCallback(
    const TcpConnectionPtr&,
    Buffer* buf) {
    buf->RetrieveAll();
}

TcpConnection::TcpConnection(
    EventLoop* loop,
    const std::string name,
    int sockfd) : 
    own_loop_(loop),
    name_(name),
    status_(CONNECTIONG),
    socket_(new TcpIPv4Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    highwater_mark_(64*1024*1024),
    connection_callback_(DefaultConnectionCallback),
    message_callback_(DefaultMessageCallback) {
    channel_->SetReadCallback(
        std::bind(&TcpConnection::HandleRead, this));
    channel_->SetWriteCallback(
        std::bind(&TcpConnection::HandleWrite, this));
    channel_->SetCloseCallback(
        std::bind(&TcpConnection::HandleClose, this));
    channel_->SetErrorCallback(
        std::bind(&TcpConnection::HandleError, this));
    
    LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
        << " fd=" << sockfd;
    socket_->SetKeepAlive(true);
}


TcpConnection::~TcpConnection() {
    LOG_INFO << "TcpConnection::dtor[" <<  name_ << "] at " << this
        << " fd = " << channel_->GetFd()
        << " status = " << StatusToString();
}

void TcpConnection::SetConnectionCallback(const ConnectionCallback& cb) {
    connection_callback_ = cb;
}

void TcpConnection::SetMessageCallback(const MessageCallback& cb) {
    message_callback_ = cb;
}

void TcpConnection::SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    write_complete_callback_ = cb;
}

void TcpConnection::SetCloseCallback(const CloseCallback& cb) {
    close_callback_ = cb;   
}

void TcpConnection::SetHighWaterMarkCallback(
    const HighWaterMarkCallback& cb,
    size_t high_water) {
    highwater_mark_callback_ = cb;
    highwater_mark_ = high_water;
}

EventLoop* TcpConnection::GetOwnLoop() const {
    return own_loop_;
}

const std::string TcpConnection::GetName() const {
    return name_;
}

const InetAddressIPV4 TcpConnection::GetLocalAddress() const {
    InetAddressIPV4 adress(0, "127.0.0.1");
    Status status = socket_->GetLocalAdress(&adress);
    return adress;
}

const InetAddressIPV4 TcpConnection::GetPeerAddress() const {
    InetAddressIPV4 adress(0, "127.0.0.1");
    Status status = socket_->GetPeerAdress(&adress);
    return adress;
}

bool TcpConnection::IsConnected() const {
    return status_ == CONNECTED;
}

bool TcpConnection::IsDisconnected() const {
    return status_ == DISCONNECTED;
}

void TcpConnection::Send(const char* message, int len) {
    Buffer local_buffer;
    local_buffer.Append(message, len);
    Send(&local_buffer);
}

void TcpConnection::Send(Buffer* message) {
    if (CONNECTED == status_) {
        if (own_loop_->IsInLoopThread()) {
            SendInLoop(static_cast<const void*>(message->Peek()), message->ReadableBytes());
            message->RetrieveAll();
        } else {
            own_loop_->RunInLoop(std::bind(
                &TcpConnection::SendInLoopA,
                shared_from_this(),
                message->RetrieveAllAsString()));
        }
    }
}

void TcpConnection::Shutdown() {
    if (CONNECTED == status_) {
        SetConnectStatus(DISCONNECTING);
        own_loop_->RunInLoop(
            std::bind(&TcpConnection::ShutdownInLoop, shared_from_this()));
    }
}

void TcpConnection::ShutdownInLoop() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }

    if (!channel_->IsWriting()) {
        socket_->ShutDownWrite();
    }
}

void TcpConnection::ForceClose() {
    if (status_ == CONNECTED || status_ == DISCONNECTING) {
        SetConnectStatus(DISCONNECTING);
        own_loop_->RunInLoop(
            std::bind(&TcpConnection::ForceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::ForceCloseInLoop() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }

    if (status_ == CONNECTED || status_ == DISCONNECTING) {
        HandleClose();
    }
}


const std::string TcpConnection::StatusToString() {
  switch (status_) {
    case DISCONNECTED:
      return "Disconnected";
    case DISCONNECTING:
      return "DisConnecting";
    case CONNECTED:
      return "Connected";
    case CONNECTIONG:
      return "Disconnecting";
    default:
      return "unknown status";
  }
}

void TcpConnection::ConnectEstablished() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }
    if (CONNECTIONG != status_) {
        LOG_FATAL << "not in connecting status.";
    }
    SetConnectStatus(CONNECTED);
    channel_->EnableReading(true);

    connection_callback_(shared_from_this());
}

void TcpConnection::ConnectDestroyed() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }
    if (CONNECTED == status_) {
        SetConnectStatus(DISCONNECTED);
        channel_->DisableAll();
        connection_callback_(shared_from_this());
    }
    channel_->RemoveSelfInPoll();
}

void TcpConnection::HandleRead() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }
    int savedErrno = 0;
    ssize_t count = input_buffer_.ReadFd(channel_->GetFd(), &savedErrno);
    if (count > 0) {
        std::cout << "count > 0" << std::endl;
        message_callback_(shared_from_this(), &input_buffer_);
    }
    else if (0 == count) {
        std::cout << "count = 0" << std::endl;
        HandleClose();
    } else {
        std::cout << "count < 0" << std::endl;
        LOG_SYSERR << "TcpConnection::handleRead";
        HandleError();
    }
}

void TcpConnection::HandleWrite() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }

    if (channel_->IsWriting()) {
        ssize_t writed = socket_->Write(
                static_cast<const char*>(output_buffer_.Peek()),
                output_buffer_.ReadableBytes());
        if (writed > 0) {
            output_buffer_.Retrieve(writed);
            if (output_buffer_.ReadableBytes() == 0) {
                channel_->EnableWriting(false);
                if (write_complete_callback_) {
                    own_loop_->RunInLoop(std::bind(write_complete_callback_, shared_from_this()));
                }
                if (DISCONNECTING == status_) {
                    ShutdownInLoop();
                }
            }
        } else {
            LOG_SYSERR << "TcpConnection::handleWrite";
        }
    } else {
        LOG_INFO << "Connection fd = " << channel_->GetFd() << " is down, no more writing";
    }
}

void TcpConnection::HandleClose() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }
    
    ConnectDestroyed();

    SetConnectStatus(DISCONNECTED);
    channel_->DisableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connection_callback_(guardThis);
    close_callback_(guardThis);
    std::cout << "ssssssssss" << std::endl;
}

void TcpConnection::HandleError() {
    int error = socket_->GetErrorCode();
    LOG_ERROR << "TcpConnection::handleError [" << name_ << "] - SO_ERROR = "
        << error << " " << ::strerror(error);
}

void TcpConnection::SetConnectStatus(const ConnectStatus status) {
    status_ = status;
}

void TcpConnection::SendInLoopA(std::string str) {
    SendInLoop(str.c_str(), str.size());
}

void TcpConnection::SendInLoop(const void* message, size_t len) {
    if (own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in thread which belong to tcpconnection.";
    }

    if (CONNECTED == status_) {
        LOG_ERROR << "tcp connection have connected, give up writing.";
        return;
    }

    ssize_t writed = 0;
    ssize_t remainning = 0;
    bool fault_error = false;
    if (!channel_->IsWriting() && output_buffer_.ReadableBytes() == 0) {
        writed = socket_->Write(static_cast<const char *>(message), len);
        if (writed >= 0) {
            remainning = len - writed;
            if (0 == remainning && write_complete_callback_) {
                own_loop_->RunInLoop(
                        std::bind(write_complete_callback_, shared_from_this()));
            }
        } else {
            writed = 0;
            if (EAGAIN != errno) {
                LOG_SYSERR << "socet write have occered error.";
                fault_error = true;
            }
        }
    }

    if (!fault_error && remainning > 0) {
        size_t old_len = output_buffer_.ReadableBytes();
        if (old_len + remainning >= highwater_mark_ && 
            highwater_mark_callback_) {
            own_loop_->RunInLoop(std::bind(
                highwater_mark_callback_,
                shared_from_this(),
                old_len + remainning));
        }
        output_buffer_.Append(
            static_cast<const char*>(message) + writed,
            remainning);
        if (!channel_->IsWriting()) {
            channel_->EnableWriting(true);
        }
    }
}

}
