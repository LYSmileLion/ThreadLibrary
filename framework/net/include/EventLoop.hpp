#ifndef FRAMEWORK_NET_INCLUDE_EVENTLOOP_HPP_
#define FRAMEWORK_NET_INCLUDE_EVENTLOOP_HPP_

#include <nocopyable.hpp>
#include <MutexLock.hpp>

namespace Net {

class Channel;
class Poller;

class EventLoop : Base::nocopyable {
 public:
    typedef std::function<void()> Task;

    EventLoop();

    ~EventLoop();

    void Loop();

    void Quit();

    void RunInLoop(Task task);

    void QueueInLoop(Task task);

    int QueueSize();

    // channel about
    void UpdateChannel(Channel *channel);

    void RemoveChannel(Channel *channel);

    bool HasChannel(Channel *channel);

 private:
    void WakeUP();

    void HandleRead();

    void DoPendingTask();

    void PrintActiveChannels();

 private:
    typedef std::vector<Channel *> ChannelList;

    std::unique_ptr<Poller> poller_;

    int wakeup_fd_;

    bool doing_task_;

    bool loop_;

    bool quit_;

    std::unique_ptr<Channel> wakeup_channel_;

    ChannelList active_channels_;

    Channel *current_active_channel_;

    Base::MutexLock task_mutex_;

    std::vector<Task> task_;
};

}

#endif
