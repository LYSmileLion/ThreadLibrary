#ifndef FRAMEWORK_NET_INCLUDE_INETADDRESS_HPP_
#define FRAMEWORK_NET_INCLUDE_INETADDRESS_HPP_

#include <stdint.h>
#include <netinet/in.h>

#include <string>

#include <nocopyable.hpp>
#include <Types.hpp>

namespace Net {

class InetAddressIPV4 {
 public:
    explicit InetAddressIPV4(uint16_t port, std::string address);

    explicit InetAddressIPV4(sockaddr_in &socket_addr);

    ~InetAddressIPV4() {}

    std::string GetIPAndPort() const;

    uint16_t GetPort() const;

    sockaddr_in GetSocketAddr() const;
 private:
    uint16_t port_;

    std::string address_;
};

Status SocketAddrToInetAddress(sockaddr_in &addr_in, InetAddressIPV4 *addr_out);

}



#endif
