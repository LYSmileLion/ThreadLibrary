#include <InetAddress.hpp>
#include <Socket.hpp>
#include <Types.hpp>
#include <Logging.hpp>

#include <string>

using namespace Net;

int main() {
    InetAddressIPV4 address(9000, std::string("127.0.0.1"));
    TcpIPv4Socket socket;
    Status status = socket.BindAddress(address);
    if (SUCCESS != status) {
        LOG_ERROR << "socket bind address failed.";
        return -1;
    }
    status = socket.Listen();
    if (SUCCESS != status) {
        LOG_ERROR << "socket listen failed.";
        return -1;
    }
    while(1) {
        int accept_id;
        status = socket.Accept(&accept_id);
        if (SOCKET_ACCEPT_FAILED == status) {
            LOG_WARN << "accept a bad socket fd.";
        } else if (SUCCESS == status) {
        } else {
            LOG_ERROR << "accept occored a error.";
            break;
        }
        TcpIPv4Socket accept_socket(accept_id);
        char  str[] = "create link success^_^.";
        ssize_t write_len = accept_socket.Write(static_cast<void *>(str), sizeof(str));
        LOG_INFO << "write size : " << write_len;
        accept_socket.Close();
    }
    return 0;    
}

