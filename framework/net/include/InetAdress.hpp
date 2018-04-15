#ifndef FRAMEWORK_NET_INCLUDE_INETADDRESS_HPP_
#define FRAMEWORK_NET_INCLUDE_INETADDRESS_HPP_

#include <stdint.h>

#include <string>

#include <nocopyable.hpp>

namespace Net {

class InetAdressIPV4 {
 public:
    explicit InetAdressIPV4(uint16_t port, std::string address);

    explicit InetAdressIPV4(sockaddr_in &socket_addr);

    ~InetAdressIPV4() {}

    std::string GetIPAndPort() const;

    uint16_t GetPort() const;

    sockaddr_in GetSocketAddr() const;
 private:
    uint16_t port_;

    std::string address_;
};

Status SocketAddrToInetAdress(sockaddr_in &addr_in, InetAdressIPV4 *addr_out);

}



#endif
