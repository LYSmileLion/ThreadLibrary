#include <EventLoop.hpp>
#include <Logging.hpp>

namespace Net {

#define POLLWAITTIME 1000

EventLoop::EventLoop() :
    doing_task_(false),
    loop_(false),
    quit_(false) {
    
}

EventLoop::~EventLoop() {
    
}

EventLoop::Loop() {
    if (!IsInLoopThread()) {
        LOG_FATAL << "loop not in loop thread.";
    }
    if (true == loop_) {
        LOG_FATAL << "loop start than one time.";
    }
    loop_ = true;
    quit_ = false;
    while (!quit_) {
        active_channels_.clear();
        poller_->poll()
    }

}

void EventLoop::RunInLoop(Task task) {
    if (IsInLoopThread()) {
        task();
    } else {
        QueueInLoop(task);
    }
}

void EventLoop::QueueInLoop(Task task) {
    {
        Base::MutexLockGuard lock(task_mutex_);
        task_.push_back(std::move(task));
    }
    if (!IsInLoopThread() || doing_task_) {
        WakeUP();
    }
}

};
