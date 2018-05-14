#ifndef FRAMEWORK_NET_INCLUDE_POLL_HPP_
#define FRAMEWORK_NET_INCLUDE_POLL_HPP_

#include <map>
#include <vector>

#include <nocopyable.hpp>
#include <Channel.hpp>
#include <EventLoop.hpp>

namespace Net {

class Poller : Base::nocopyable {
 public:
    typedef std::vector<Channel *> ChannelList;

    Poller(EventLoop* loop);

    ~Poller();

    void Poll(int timeoutMs, ChannelList* activeChannels);

    void UpdateChannel(Channel* channel);

    void RemoveChannel(Channel* channel);

    bool HasChannel(Channel *channel);

 private:
    typedef std::vector<struct pollfd> PollFdList;

    typedef std::map<int, Channel*> ChannelMap;

    void FillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    PollFdList pollfds_;

    ChannelMap channels_;

    EventLoop *own_loop_;
};

}
#endif
