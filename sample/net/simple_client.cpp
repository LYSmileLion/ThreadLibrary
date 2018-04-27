#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>

#include <string>
#include <iostream>

using namespace Net;

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    TcpIPv4Socket socket;
    Status status = socket.Connect(address);
    if (SUCCESS != status) {
        LOG_ERROR << "socket listen failed.";
        return -1;
    }
    char buf[100];
    socket.Read(static_cast<void *>(buf), sizeof(buf));
    std::cout << buf << std::endl;
    return 0;    
}

