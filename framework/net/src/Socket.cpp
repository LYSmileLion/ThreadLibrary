#include <sys/socket.h>

#include <Socket.hpp>

namespace Net {

TcpIPv4Socket::TcpIPv4Socket(int soket_fd) :
    errno_(0),
    socket_fd_(soket_fd) {}

TcpIPv4Socket::TcpIPv4Socket() :
    errno_(0) {
    socket_fd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (-1 == socket_fd_) {
        std::cout << "create socket fd failed." << std::endl;    
    }
}

TcpIPv4Socket::~TcpIPv4Socket() {}

Status TcpIPv4Socket::BindAddress(const InetAdressIPV4& adress) {
    if (0 != errno_) {
        std::cout << "socket create failed." << std::endl;
        return SOCKET_CREATE_FAILED;
    }
    sockaddr_in socket_address = address.GetSocketAddr();
    int status = ::bind(
        socket_fd_, 
        static_cast<sockaddr *>(&socket_address),
        sizeof(socket_address));
    if (0 != status) {
        std::cout << "socket bind failed." << std::endl;
        return SOCKET_BIND_FAILED;    
    }
    return SUCCESS;
}

Status TcpIPv4Socket::Listen() {
    if (0 != errno_) {
        std::cout << "socket create failed." << std::endl;
        return SOCKET_CREATE_FAILED;    
    }   
    int status = ::listen(socket_fd_, SOMAXCONN);
}

    Status Accept(int *accept_fd);

    Status Close();

    Status ShutDownWrite();

    Status Read(void *buf, size_t count);

    Status Write(void *buf, size_t count);

    Status GetLocalAdress(InetAdressIPV4 *adress);

    Status GetPeerAdress(InetAdressIPV4 *adress);

    int GetErrorCode() const;

 private:
    int errno_;

    int socket_fd_;
};

Status IsSelfConnect(const TcpIPv4Socket &socket1, const TcpIPv4Socket &soket2);

}

#endif
