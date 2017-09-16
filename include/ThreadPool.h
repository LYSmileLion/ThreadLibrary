#ifdef  _INCLUDE_THREADPOOL_HPP_
#define _INCLUDE_THREADPOOL_HPP_

#include <functional>
#include <noncopyable>

namespace HPCs {//high performance 
class ThreadPool {
 public:
    typedef std::function<void ()> Task;

    explicit ThreadPool(const std::string name = std::string("ThreadPool"));

    void setTaskQueueSize(const max_size);

    void setThreadInitCallBack(const Task& task);

    void start(const int thread_num);

    void stop();

    const string name() const;

    int taskQueueSize() const;

    void runTask(const Task& task);

 private:
    bool isTaskQueueFull() const;

    void runInThread();

    Task ta
 
 };
}
#endif
