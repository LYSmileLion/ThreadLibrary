#ifndef FRAMEWORK_NET_INCLUDE_CHANNEL_HPP_
#define FRAMEWORK_NET_INCLUDE_CHANNEL_HPP_

#include <poll.h>

#include <functional>
#include <memory>
#include <string>

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

    void SetCloseCallback(EventCallback cb);

    void SetErrorCallback(EventCallback cb);

    void EnableReading(bool status);

    void EnableWriting(bool status);

    void DisableAll();

    bool IsReading() const;

    bool IsWriting() const;

    bool IsNoneEvent() const;

    int GetIntersetEvents() const;

    void HandleEvent();

    void SetCurrentEvents(int events);
    
    int GetFd() const;

    EventLoop *OwnerLoop() const;

    std::string EventsToString();
    //poll use
    void RemoveSelfInPoll();

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

    bool added_to_loop_;

    EventCallback read_callBack_;

    EventCallback write_callBack_;

    EventCallback close_callBack_;

    EventCallback error_callBack_;
};

}

#endif
