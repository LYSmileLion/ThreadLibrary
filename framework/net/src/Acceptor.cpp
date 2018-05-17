#include <Acceptor.hpp>
#include <Logging.hpp>
#include <EventLoop.hpp>
#include <InetAddress.hpp>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

namespace Net {

Acceptor::Acceptor(EventLoop* loop, const InetAddressIPV4 &address) :
    own_loop_(loop),
    address_(address),
    socket_(),
    channel_(loop, socket_.GetSocketFd()),
    idle_fd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {}

Acceptor::~Acceptor() {
    channel_.DisableAll();
    channel_.RemoveSelfInPoll();
    ::close(idle_fd_);
}

void Acceptor::Start() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }
    socket_.SetReuseAddr(true);
    socket_.BindAddress(address_);
    channel_.SetReadCallback(std::bind(&Acceptor::HandleRead, this));
    socket_.Listen();
    channel_.EnableReading(true);
}

void Acceptor::SetNewConnectionCallback(const NewConnectionCallback& cb) {
    newconnection_callback_ = cb;    
}

void Acceptor::HandleRead() {
    if (!own_loop_->IsInLoopThread()) {
        LOG_FATAL << "not in loop thread which tcp connection belong to.";
    }
    int connect_fd = 0;
    Status status = socket_.Accept(&connect_fd);
    if (SUCCESS == status) {
        if (newconnection_callback_) {
            newconnection_callback_(connect_fd);
        } else {
            TcpIPv4Socket socket(connect_fd);
        }
    } else {
        LOG_SYSERR << "in Acceptor::handleRead";
        if (errno == EMFILE) {
            ::close(idle_fd_);
            idle_fd_ = ::accept(socket_.GetSocketFd(), NULL, NULL);
            ::close(idle_fd_);
            idle_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}
}
