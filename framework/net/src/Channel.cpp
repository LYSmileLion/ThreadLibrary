#include <sstream>

#include <Logging.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>

namespace Net {

Channel::Channel(EventLoop *loop, int fd) :
    local_loop_(loop),
    fd_(fd),
    interested_event_(NONEEVENT),
    current_event_(NONEEVENT),
    poll_index_(-1),
    added_to_loop_(false) {}

Channel::~Channel() {}

void Channel::SetReadCallback(EventCallback cb) {
    read_callBack_ = std::move(cb);
}

void Channel::SetWriteCallback(EventCallback cb) {
    write_callBack_ = std::move(cb);
}

void Channel::SetCloseCallback(EventCallback cb) {
    close_callBack_ = std::move(cb);
}

void Channel::SetErrorCallback(EventCallback cb) {
    error_callBack_ = std::move(cb);
}

void Channel::EnableReading(bool status) {
    if (true == status) {
        interested_event_ |= READEVENT;
    } else {
        interested_event_ &= (~READEVENT);
    }
    UpdatePollList();
}

void Channel::EnableWriting(bool status) {
    if (true == status) {
        interested_event_ |= WRITEEVENT;
    } else {
        interested_event_ &= (~WRITEEVENT);
    }
    UpdatePollList();
}

bool Channel::IsReading() const {
    return (interested_event_ & READEVENT);
}

bool Channel::IsWriting() const {
    return (interested_event_ & WRITEEVENT);
}

bool Channel::IsNoneEvent() const {
    return (interested_event_ == NONEEVENT);
}

void Channel::DisableAll() {
    interested_event_ = NONEEVENT;
    UpdatePollList();
}

int Channel::GetIntersetEvents() const {
    return interested_event_;
}

void Channel::HandleEvent() {
    if ((current_event_ & POLLHUP) && !(current_event_ & POLLIN)) {
        if (close_callBack_) {
            close_callBack_();
        }
    }

    if (current_event_ & POLLNVAL) {
        LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLNVAL";
    }

    if (current_event_ & (POLLERR | POLLNVAL)) {
        if (error_callBack_) {
            error_callBack_();
        }
    }

    if (current_event_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (read_callBack_) {
            read_callBack_();
        }
    }

    if (current_event_ & POLLOUT) {
        if (write_callBack_) {
            write_callBack_();
        }
    }
}

void Channel::SetCurrentEvents(int events) {
    current_event_ = events;   
}

int Channel::GetFd() const {
    return fd_; 
}

EventLoop* Channel::OwnerLoop() const {
    return local_loop_;
}

void Channel::RemoveSelfInPoll() {
    added_to_loop_ = false;
    local_loop_->RemoveChannel(this);
}

int Channel::GetPollIndex() {
    return poll_index_;
}

void Channel::SetPollIndex(int index) {
    poll_index_ = index;
}
void Channel::UpdatePollList() {
    added_to_loop_ = true;
    local_loop_->UpdateChannel(this);
}

std::string Channel::EventsToString() {
    std::ostringstream output;
    output << fd_ << " : ";
    if (current_event_ & POLLIN) {
        output << "IN ";
    }
    if (current_event_ & POLLPRI) {
        output << "PRI ";
    }
    if (current_event_ & POLLOUT) {
        output << "OUT ";
    }
    if (current_event_ & POLLHUP) {
        output << "HUP ";
    }
    if (current_event_ & POLLRDHUP) {
        output << "RDHUP ";
    }
    if (current_event_ & POLLERR) {
        output << "ERR ";
    }
    if (current_event_ & POLLNVAL) {
        output << "NVAL ";
    }
    return output.str().c_str();
}
}
