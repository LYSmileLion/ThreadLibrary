#ifndef FRAMEWORK_NET_INCLUDE_TCPCONNECTION_HPP_
#define FRAMEWORK_NET_INCLUDE_TCPCONNECTION_HPP_

#include <memory>
#include <string>
#include <functional>

#include <Types.hpp>
#include <InetAddress.hpp>
#include <Socket.hpp>
#include <nocopyable.hpp>
#include <Buffer.hpp>

namespace Net {

class Channel;
class EventLoop;
class Socket;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*)> MessageCallback;

void DefaultConnectionCallback(const TcpConnectionPtr& connection);
void DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf);

class TcpConnection : public Base::nocopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
    TcpConnection(
        EventLoop* loop,
        const std::string name,
        int sockfd);

    ~TcpConnection();

    void SetConnectionCallback(const ConnectionCallback& cb);

    void SetMessageCallback(const MessageCallback& cb);

    void SetWriteCompleteCallback(const WriteCompleteCallback& cb);

    void SetHighWaterMarkCallback(
        const HighWaterMarkCallback& cb,
        size_t highWaterMark);

    void SetCloseCallback(const CloseCallback& cb);

    EventLoop *GetOwnLoop() const;

    const std::string GetName() const;

    const InetAddressIPV4 GetLocalAddress() const;

    const InetAddressIPV4 GetPeerAddress() const;

    bool IsConnected() const;

    bool IsDisconnected() const;

    void Send(const char* message, int len);

    void Send(Buffer* message);

    void Shutdown();

    void ForceClose();

    void ConnectEstablished();

    void ConnectDestroyed();

 private:
    typedef enum ConnectStatus {
        DISCONNECTING   = 0,
        DISCONNECTED    = 1,
        CONNECTIONG     = 2,
        CONNECTED       = 3,
    } ConnectStatus;

    void ShutdownInLoop();

    void ForceCloseInLoop();

    void HandleRead();

    void HandleWrite();

    void HandleClose();

    void HandleError();

    void SendInLoop(const void* message, size_t len);

    void SendInLoopA(std::string str);

    void SetConnectStatus(const ConnectStatus status);

    const std::string StatusToString();

 private:
    EventLoop* own_loop_;

    const std::string name_;

    ConnectStatus status_;

    size_t highwater_mark_;

    std::unique_ptr<TcpIPv4Socket> socket_;

    std::unique_ptr<Channel> channel_;

    ConnectionCallback connection_callback_;

    MessageCallback message_callback_;

    WriteCompleteCallback write_complete_callback_;

    HighWaterMarkCallback highwater_mark_callback_;

    CloseCallback close_callback_;

    Buffer input_buffer_;

    Buffer output_buffer_;
};


}
#endif
