#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <sstream>

#include <InetAddress.hpp>
#include <Logging.hpp>


namespace Net {
InetAddressIPV4::InetAddressIPV4(uint16_t port, std::string address):
    port_(port),
    address_(address) {}

InetAddressIPV4::InetAddressIPV4(sockaddr_in &socket_addr) {
    
}

std::string InetAddressIPV4::GetIPAndPort() const {
    std::ostringstream stream;
    stream << port_;
    return (address_ + " : " + stream.str());
}

uint16_t InetAddressIPV4::GetPort() const {
    return port_;    
}

sockaddr_in InetAddressIPV4::GetSocketAddr() const {
    sockaddr_in socket_ipv4;
    bzero(&socket_ipv4, sizeof(socket_ipv4));
    socket_ipv4.sin_port = port_;
    socket_ipv4.sin_family = AF_INET;
    ::inet_pton(AF_INET, address_.c_str(), &socket_ipv4.sin_addr);
    return socket_ipv4;
}


Status SocketAddrToInetAddress(sockaddr_in &addr_in, InetAddressIPV4 *addr_out) {
    uint16_t port = ntohs(addr_in.sin_port);
    char buf[INET_ADDRSTRLEN];
    const char *address = ::inet_ntop(
        AF_INET, 
        reinterpret_cast<const void *>(&addr_in.sin_addr),
        buf,
        sizeof(buf));
    if (NULL == address) {
        LOG_ERROR << "convert ip address failed.";
        return SOCKET_ADDRESS_CONVERT_FAILED;
    }
    *addr_out = InetAddressIPV4(port, std::string(address));
    return SUCCESS;
}
}
