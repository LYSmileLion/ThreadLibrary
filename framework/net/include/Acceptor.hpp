#ifndef FRAMEWORK_NET_INCLUDE_ACCEPTOR_HPP_
#define FRAMEWORK_NET_INCLUDE_ACCEPTOR_HPP_

#include <functional>

#include <Channel.hpp>
#include <Socket.hpp>
#include <nocopyable.hpp>

namespace Net {

class EventLoop;
class InetAddress;

class Acceptor : Base::nocopyable {
 public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddressIPV4 &address);

    ~Acceptor();

    void SetNewConnectionCallback(const NewConnectionCallback& cb);

    void Start();

 private:
    void HandleRead();

    EventLoop* own_loop_;

    InetAddressIPV4 address_;

    TcpIPv4Socket socket_;

    Channel channel_;

    int idle_fd_;

    NewConnectionCallback newconnection_callback_;

    bool listenning_;
};

}

#endif
