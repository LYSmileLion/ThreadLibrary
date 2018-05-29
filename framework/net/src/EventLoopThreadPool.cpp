#include <EventLoopThreadPool.hpp>
#include <Logging.hpp>

namespace Net {

EventLoopThreadPool::EventLoopThreadPool(EventLoop *base) :
    base_(base),
    thread_num_(0),
    current_index_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::SetThreadNum(int value) {
    thread_num_ = value;
}

void EventLoopThreadPool::Start() {
    thread_pool_.start(thread_num_);
    for (int i = 0; i < thread_num_; i++) {
        thread_pool_.runTask(
            std::bind(&EventLoopThreadPool::Task, this));
    }
}

EventLoop *EventLoopThreadPool::GetNextLoop() {
    if (0 == thread_num_) {
        return base_;
    } else {
        EventLoop *loop = eventloop_vec[current_index_].get();
        current_index_++;
        if (thread_num_ > current_index_) {
            current_index_ = 0;
        }
        return loop;
    }
}

void EventLoopThreadPool::Task() {
    eventloop_vec.emplace_back(new EventLoop());
    (eventloop_vec.back())->Loop();
}
}

