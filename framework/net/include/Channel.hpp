#ifndef FRAMEWORK_NET_INCLUDE_CHANNEL_HPP_
#define FRAMEWORK_NET_INCLUDE_CHANNEL_HPP_

#include <functional>
#include <memory>

#include <nocopyable.hpp>

namespace Net {
class EventLoop;
class Channel : Base::nocopyable {
 public:
    typedef std::function<void()> EventCallback;
    
    Channel(EventLoop *loop, int fd);

    ~Channel();

    void SetReadCallback(EventCallback cb);

    void SetWriteCallback(EventCallback cb);

    void setCloseCallback(EventCallback cb);

    void setErrorCallback(EventCallback cb);

    void EnableReading(bool status);

    void EnableWriting(bool status);

    void DisableAll();

    bool IsReading() const;

    bool IsWriting() const;

    void HandleEvent();

    //poll use
    int GetPollIndex();

    void SetPollIndex(const int index);

 private:
    void UpdatePollList();

 private:
    enum {
        NONEEVENT       = 0,
        READEVENT       = POLLIN | POLLPRI,
        WRITEEVENT      = POLLOUT
    };

    EventLoop *local_loop_;

    const int fd_;

    int interested_event_;

    int current_event_;

    int poll_index_;

    EventCallback read_callBack_;

    EventCallback write_callBack_;

    EventCallback close_callBack_;

    EventCallback error_callBack_;
};

}

#endif
