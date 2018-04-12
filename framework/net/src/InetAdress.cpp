#include <netinet/in.h>
#include <arpa/inet.h>

#include <sstream>

#include <InetAdress.hpp>


namespace Net {
InetAdressIPV4::InetAdressIPV4(uint16_t port, std::string address):
    port_(port),
    address_(address) {}

InetAdressIPV4::InetAdressIPV4(sockaddr_in &socket_addr) {
    
}

std::string InetAdressIPV4::GetIPAndPort() const {
    std::ostringstream stream;
    stream << port_;
    return (address_ + " : " + stream.str());
}

uint16_t InetAdressIPV4::GetPort() const {
    return port_;    
}

sockaddr_in InetAdressIPV4::GetSocketAddr() const {
    sockaddr_in socket_ipv4;
    bzero(&socket_ipv4, sizeof(socket_ipv4));
    socket_ipv4.sin_family = AF_INET;
    ::inet_pton(AF_INET, address_.str(), &socket_ipv4.sin_addr);
    return socket_ipv4;
}


Status SocketAddrToInetAdress(sockaddr_in &addr_in, InetAdressIPV4 *addr_out) {
    uint16_t port = ::ntohs(addr_in.port);
    char buf[INET_ADDRSTRLEN];
    char *address = ::inet_ntop(AF_INET, &(addr_in.sin_addr), buf, sizeof(buf));
    if (NULL == address) {
        std::cout << "convert ip address failed." << std::endl;
        return SOCKET_ADDRESS_CONVERT_FAILED;
    }
    *addr_out = InetAdressIPV4(port, std::string(address));
    return SUCCESS;
}
}
