#ifndef FRAMEWORK_NET_INCLUDE_SOCKET_HPP_
#define FRAMEWORK_NET_INCLUDE_SOCKET_HPP_

#include <InetAddress.hpp>

namespace Net {
//this calss create socket for tcp ipv4,and the socket is noblock
class TcpIPv4Socket : Base::nocopyable {
 public:
    TcpIPv4Socket(int soket_fd);

    TcpIPv4Socket();

    ~TcpIPv4Socket();

    Status BindAddress(const InetAddressIPV4& address);

    Status Listen();

    Status Accept(int *accept_fd);

    Status Close();

    Status ShutDownWrite();

    Status Connect(const InetAddressIPV4 &address);

    ssize_t Read(void *buf, size_t count);

    ssize_t Write(void *buf, size_t count);

    Status GetLocalAdress(InetAddressIPV4 *adress);

    Status GetPeerAdress(InetAddressIPV4 *adress);

    int GetErrorCode() const;

 private:
    int socket_fd_;
};

}

#endif
