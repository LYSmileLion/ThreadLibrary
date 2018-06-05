#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>

#include <string>
#include <iostream>

#include<unistd.h>

using namespace Net;

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    TcpIPv4Socket socket(true);
    Status status = socket.Connect(address);
    if (SUCCESS != status) {
        LOG_ERROR << "socket listen failed.";
        return -1;
    }
    char *str = "sdasdasdasdas";
    socket.Write(static_cast<void *>(str), sizeof(str));
    sleep(20);
    socket.ShutDownWrite();
    while(1);
    return 0;    
}

