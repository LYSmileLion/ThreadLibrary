#ifndef  _INCLUDE_THREADPOOL_HPP_
#define _INCLUDE_THREADPOOL_HPP_

#include <functional>
#include <nocopyable.hpp>

namespace HPCs {//high performance 

class ThreadPool {
 public:
    typedef std::function<void ()> Task;

    explicit ThreadPool(const std::string name = std::string("ThreadPool"));

    void setTaskQueueSize(const int max_size);

    void setThreadInitCallBack(const Task& task);

    void start(const int thread_num);

    void stop();

    const string name() const;

    int taskQueueSize() const;

    void runTask(const Task& task);

 private:
    bool isFull();

    bool isTaskQueueFull() const;

    void runInThread();

    mutable MutexLock mutex_;

    Condition notEmpty_;

    Condition notFull_;

    Task threadInitCallback_;

    std::vector<std::shared_ptr<Thread>> threads_;

    std::deque<Task> queue_;

    bool running_;

    int max_queue_size;
 };
}
#endif
