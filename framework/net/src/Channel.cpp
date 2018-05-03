#include <Channel.hpp>

namespace Net {

Channel::Channel(EventLoop *loop, int fd) {
    
}

Channel::~Channel() {}

void Channel::SetReadCallback(EventCallback cb) {
    readCallback_ = std::move(cb);
}

void Channel::SetWriteCallback(EventCallback cb) {
    writeCallback_ = std::move(cb);
}

void Channel::setCloseCallback(EventCallback cb) {
    closeCallback_ = std::move(cb);
}

void Channel::setErrorCallback(EventCallback cb) {
    errorCallback_ = std::move(cb);
}

void Channel::EnableReading(bool status) {
    if (true == status) {
        interested_event_ |= READEVENT;
        UpdatePollList();
    } else {
        interested_event_ |= (~READEVENT);
        UpdatePollList();
    }
}

void Channel::EnableWriting(bool status) {
    if (true == status) {
        interested_event_ |= WRITEEVENT;
        UpdatePollList();
    } else {
        interested_event_ |= (~WRITEEVENT);
        UpdatePollList();
    }
}

bool Channel::IsReading() const {
    return (interested_event_ & READEVENT);
}

bool Channel::IsWriting() const {
    return (interested_event_ & WRITEEVENT);
}

void Channel::HandleEvent() {
    

}
}
