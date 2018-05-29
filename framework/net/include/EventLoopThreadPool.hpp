#ifndef FRAMEWORK_NET_INCLUDE_EVENTLOOPTHREADPOOL_HPP_
#define FRAMEWORK_NET_INCLUDE_EVENTLOOPTHREADPOOL_HPP_

#include <vector>
#include <memory>

#include <nocopyable.hpp>
#include <EventLoop.hpp>
#include <ThreadPool.hpp>

namespace Net {

class EventLoopThreadPool : Base::nocopyable {
 public:
    EventLoopThreadPool(EventLoop *base);

    ~EventLoopThreadPool();

    void SetThreadNum(int value);

    void Start();

    EventLoop *GetNextLoop();

 private:
    void Task();

 private:
    Threads::ThreadPool thread_pool_;

    std::vector<std::unique_ptr<EventLoop>> eventloop_vec;

    EventLoop *base_;

    int thread_num_;

    int current_index_;
};

}

#endif
