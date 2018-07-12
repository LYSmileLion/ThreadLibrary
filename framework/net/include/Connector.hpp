#ifndef FRAMEWORK_NET_INCLUDE_CONNECTOR_HPP_
#define FRAMEWORK_NET_INCLUDE_CONNECTOR_HPP_

#include <functional>
#include <memory>
#include <atomic>

#include <InetAddress.hpp>
#include <nocopyable.hpp>

namespace Net {

class Channel;
class EventLoop;

class Connector :
    public Base::nocopyable,
    public std::enable_shared_from_this<Connector> {
 public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddressIPV4& server);

    ~Connector();

    void SetNewConnectionCallback(const NewConnectionCallback& callback);

    void Start();

    void Restart();

    void Stop();

    const InetAddressIPV4 ServerAddress() const;

 private:
    typedef enum ConnectStatus {
        DISCONNECTED    = 0,
        CONNECTING      = 1,
        CONNECTED       = 2,
    } ConnectStatus;

    static const int kMaxRetryDelayMs = 30*1000;

    static const int kInitRetryDelayMs = 500;

    void SetState(ConnectStatus status);

    void StartInLoop();

    void StopInLoop();

    void Connect();

    void Connecting(int sockfd);

    void HandleWrite();

    void HandleError();

    void Retry(int sockfd);

    int RemoveAndResetChannel();

    void ResetChannel();

 private:
    EventLoop* local_loop_;

    InetAddressIPV4 server_addr_;

    std::atomic<bool> connect_;

    ConnectStatus status_;

    std::unique_ptr<Channel> channel_;

    NewConnectionCallback connection_callback_;

    int retry_delay_ms_;
};

}

#endif
