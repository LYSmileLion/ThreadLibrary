#ifndef FRAMEWORK_NET_INCLUDE_TCPCLIENT_HPP_
#define FRAMEWORK_NET_INCLUDE_TCPCLIENT_HPP_

#include <string>
#include <functional>
#include <memory>
#include <atomic>

#include <MutexLock.hpp>
#include <TcpConnection.hpp>
#include <InetAddress.hpp>
#include <nocopyable.hpp>

namespace Net {

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : 
    public Base::nocopyable,
    public std::enable_shared_from_this<TcpClient> {
 public:
    TcpClient(
        EventLoop* loop,
        const std::string name,
        const InetAddressIPV4 server);

    ~TcpClient();

    void Connect();

    void Disconnect();
    
    void Stop();

    TcpConnectionPtr Connection() const;

    EventLoop* GetLoop() const;

    bool Retry() const;
    
    void EnableRetry();

    const std::string GetName();

    void SetConnectionCallback(ConnectionCallback callback);

    void SetMessageCallback(MessageCallback callback);

    void SetWriteCompleteCallback(WriteCompleteCallback callback);

 private:
    void NewConnection(int sockfd);
    
    void RemoveConnection(const TcpConnectionPtr& conn);
 
 private:
    EventLoop* local_loop_;
    
    ConnectorPtr connector_;
    
    const std::string name_;
    
    ConnectionCallback connection_callback_;
    
    MessageCallback message_callback_;
    
    WriteCompleteCallback write_complete_callback_;
    
    std::atomic<bool> retry_;
    
    std::atomic<bool> connect_;
    
    int next_connect_id_;
    
    Threads::MutexLock mutex_;
   
    TcpConnectionPtr connection_;
};

}

#endif
