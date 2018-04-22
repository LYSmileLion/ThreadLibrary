#ifndef FRAMEWORK_NET_INCLUDE_SOCKET_HPP_
#define FRAMEWORK_NET_INCLUDE_SOCKET_HPP_

#include <InetAdress.hpp>

namespace Net {
//this calss create socket for tcp ipv4,and the socket is noblock
class TcpIPv4Socket : Base::nocopyable {
 public:
    TcpIPv4Socket(int soket_fd);

    TcpIPv4Socket();

    ~TcpIPv4Socket();

    Status BindAddress(const InetAdressIPV4& adress);

    Status Listen();

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
