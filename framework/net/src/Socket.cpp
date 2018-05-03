#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <Socket.hpp>
#include <Logging.hpp>

namespace Net {

TcpIPv4Socket::TcpIPv4Socket(int soket_fd) :
    socket_fd_(soket_fd) {}

TcpIPv4Socket::TcpIPv4Socket() {
    //socket_fd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    socket_fd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if (-1 == socket_fd_) {
        LOG_SYSFATAL << "create socket fd failed.";
    }
}

TcpIPv4Socket::~TcpIPv4Socket() {
    Close();
}

Status TcpIPv4Socket::BindAddress(const InetAddressIPV4& address) {
    sockaddr_in socket_address = address.GetSocketAddr();
    int status = ::bind(
        socket_fd_, 
        reinterpret_cast<sockaddr *>(&socket_address),
        sizeof(socket_address));
    if (0 != status) {
        LOG_SYSERR << "bind socket filed.";
        return SOCKET_BIND_FAILED;
    }
    return SUCCESS;
}

Status TcpIPv4Socket::Listen() {
    int status = ::listen(socket_fd_, SOMAXCONN);
    if (status < 0) {
        LOG_SYSERR << "socket listen filed.";
        return SOCKET_LISTEN_FAILED;
    }
    return SUCCESS;
}

Status TcpIPv4Socket::Accept(int *accept_fd) {
    if (NULL == accept_fd) {
        LOG_ERROR << "param error.";
        return PARAM_ERROR;
    }
    sockaddr_in addr;
    socklen_t len = static_cast<socklen_t>(sizeof(addr));
    int connfd = ::accept(
        socket_fd_, 
        reinterpret_cast<sockaddr *>(&addr), 
        &len);
    if (connfd < 0) {
        switch (errno) {
            case EAGAIN : // 当为非阻塞的socket时,表示listen队列中没有完成链接的链接
            case ECONNABORTED : //已完成三次握手的链接,在accept之前挂掉
            case EINTR : //发生了中断
            case EPERM : //操作不允许
            case EMFILE : { // 打开的文件太多
                break;
            }
            default : {
                LOG_SYSFATAL << "accept have occored fatal system error.";
            }
        }
        return SOCKET_ACCEPT_FAILED;
    }
    *accept_fd = connfd;
    return SUCCESS;
}

Status TcpIPv4Socket::Close() {
    int status = ::close(socket_fd_);
    if (0 != status) {
        LOG_SYSERR << "socket close failed.";
    }
    return SUCCESS;
}

Status TcpIPv4Socket::ShutDownWrite() {
    int status = ::shutdown(socket_fd_, SHUT_WR);
    if (0 != status) {
        LOG_SYSERR << "socket shutdown write failed.";
    }
    return SUCCESS;
}

Status TcpIPv4Socket::Connect(const InetAddressIPV4 &address) {
    const sockaddr_in socket_address = address.GetSocketAddr();
    int status = ::connect(
        socket_fd_,
        (const sockaddr *)(&socket_address),
        static_cast<socklen_t>(sizeof(socket_address)));
    if (-1 == status) {
        LOG_SYSERR << "connect " << address.GetIPAndPort() << " failed.";
        return SOCKET_CONNECT_FAILED;
    }
    return SUCCESS;
}

ssize_t TcpIPv4Socket::Read(void *buf, size_t count) {
    return ::read(socket_fd_, buf, count);
}

ssize_t TcpIPv4Socket::Write(void *buf, size_t count) {
    return ::write(socket_fd_, buf, count);
}

Status TcpIPv4Socket::GetLocalAdress(InetAddressIPV4 *adress) {
    if (NULL == adress) {
        LOG_ERROR << "input an invalid pointer.";
        return PARAM_ERROR;
    }
    struct sockaddr_in localaddr;
    ::bzero(&localaddr, sizeof(localaddr));
    socklen_t len = static_cast<socklen_t>(sizeof(localaddr));
    int status = ::getsockname(
        socket_fd_,
        reinterpret_cast<sockaddr *>(&localaddr),
        &len);
    if (0 != status) {
        LOG_SYSERR << "socket get local name failed.";
        return SOCKET_GETLOCAK_NAME_FAILED;
    }
    *adress = InetAddressIPV4(localaddr);
    return SUCCESS;
}

Status TcpIPv4Socket::GetPeerAdress(InetAddressIPV4 *adress) {
    if (NULL == adress) {
        LOG_ERROR << "input an invalid pointer.";
        return PARAM_ERROR;
    }
    struct sockaddr_in localaddr;
    ::bzero(&localaddr, sizeof(localaddr));
    socklen_t len = static_cast<socklen_t>(sizeof(localaddr));
    int status = ::getpeername(
        socket_fd_,
        reinterpret_cast<sockaddr *>(&localaddr),
        &len);
    if (0 != status) {
        LOG_SYSERR << "socket get peer name failed.";
        return SOCKET_GETLOCAK_NAME_FAILED;
    }
    *adress = InetAddressIPV4(localaddr);
    return SUCCESS;
}

}
