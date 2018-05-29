#ifndef FRAMEWORK_NET_INCLUDE_TCPSERVER_HPP_
#define FRAMEWORK_NET_INCLUDE_TCPSERVER_HPP_

#include <functional>
#include <string>
#include <atomic>
#include <map>

#include <InetAddress.hpp>
#include <nocopyable.hpp>
#include <TcpConnection.hpp>
#include <EventLoopThreadPool.hpp>
#include <Acceptor.hpp>

namespace Net {

class TcpServer : public Base::nocopyable {
 public:
    TcpServer(EventLoop *loop, std::string name, const InetAddressIPV4 address);

    void Start(int thread_num = 0);

    void SetConnectionCallback(const ConnectionCallback& cb);

    void SetMessageCallback(const MessageCallback& cb);

    void SetWriteCompleteCallback(const WriteCompleteCallback& cb);

 private:
    void HandleNewConnection(int sockfd);

    void DefaultCloseBack(const TcpConnectionPtr&);

    void removeConnection(const TcpConnectionPtr& conn);
 private:
    typedef std::map<std::string, std::shared_ptr<TcpConnection>> ConnectMap_;

    ConnectMap_ connect_;

    Acceptor acceptor_;

    EventLoop *local_loop_;

    int next_connect_id_;

    std::atomic<bool> started_;

    std::string name_;

    EventLoopThreadPool eventloop_threadpool_;

    ConnectionCallback connect_callback_;

    MessageCallback message_callback_;

    WriteCompleteCallback write_complete_callback_;
};

}
#endif
