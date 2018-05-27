#ifndef FRAMEWORK_NET_INCLUDE_SOCKET_HPP_
#define FRAMEWORK_NET_INCLUDE_SOCKET_HPP_

#include <InetAddress.hpp>

namespace Net {
//this calss create socket for tcp ipv4, and not have socket fd
class TcpIPv4Socket : Base::nocopyable {
 public:
    TcpIPv4Socket(int soket_fd);

    TcpIPv4Socket(bool block = false);

    ~TcpIPv4Socket();

    Status SetKeepAlive(bool status);

    Status SetReuseAddr(bool on);

    Status SetTcpNoDelay(bool on);

    int GetFd() const;

    Status BindAddress(const InetAddressIPV4& address);

    Status Listen();

    Status Accept(int *accept_fd);

    Status Connect(const InetAddressIPV4 &address);

    ssize_t Read(void *buf, size_t count);

    ssize_t Write(const void *buf, size_t count);

    Status ShutDownWrite();

    Status Close();

    Status GetLocalAdress(InetAddressIPV4 *adress);

    Status GetPeerAdress(InetAddressIPV4 *adress);

    Status GetErrorCode(int *value);

 private:
    int socket_fd_;
};

}

#endif
