#include <Poller.hpp>

#include <Logging.hpp>
#include <Types.hpp>

#include <assert.h>
#include <errno.h>
#include <poll.h>

namespace Net {

Poller::Poller(EventLoop* loop)
  : own_loop_(loop) {}

Poller::~Poller() {}

void Poller::Poll(int time, ChannelList* activeChannels) {
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), -1);
    if (numEvents > 0) {
      LOG_TRACE << numEvents << " events happened";
      FillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
      LOG_TRACE << " nothing happened";
    } else {
      if (errno != EINTR) {
        LOG_SYSERR << "Poll::poll()";
      }
    }
}

void Poller::FillActiveChannels(
    int numEvents,
    ChannelList *activeChannels) const {
    for (PollFdList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && numEvents > 0; pfd++) {
      if (pfd->revents > 0) {
        --numEvents;
        ChannelMap::const_iterator iter_channel = channels_.find(pfd->fd);
        Channel* channel = iter_channel->second;
        channel->SetCurrentEvents(pfd->revents);
        activeChannels->push_back(channel);
      }
    }
}

void Poller::UpdateChannel(Channel* channel) {
  LOG_INFO << "fd = " << channel->GetFd() << " events = " << channel->EventsToString();
  if (channel->GetPollIndex() < 0) {
    // a new one, add to pollfds_
    struct pollfd pfd;
    pfd.fd = channel->GetFd();
    pfd.events = static_cast<short>(channel->GetIntersetEvents());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size())-1;
    channel->SetPollIndex(idx);
    channels_[pfd.fd] = channel;
  } else {
    int idx = channel->GetPollIndex();
    struct pollfd& pfd = pollfds_[idx];
    pfd.fd = channel->GetFd();
    pfd.events = static_cast<short>(channel->GetIntersetEvents());
    pfd.revents = 0;
    if (channel->IsNoneEvent()) {
      // ignore this pollfd
      pfd.fd = -channel->GetFd()-1;
    }
  }
}

bool Poller::HasChannel(Channel *channel) {
    ChannelMap::const_iterator iter = 
        channels_.find(channel->GetFd());
    return iter != channels_.end() && iter->second == channel;
}

void Poller::RemoveChannel(Channel* channel) {
  LOG_INFO << "fd = " << channel->GetFd();
  int idx = channel->GetPollIndex();
  const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
  size_t n = channels_.erase(channel->GetFd());(void)n;
  if (static_cast<size_t>(idx) == (pollfds_.size() - 1)) {
    pollfds_.pop_back();
  } else {
    int channelAtEnd = pollfds_.back().fd;
    iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
    if (channelAtEnd < 0) {
      channelAtEnd = -channelAtEnd -1;
    }
    channels_[channelAtEnd]->SetPollIndex(idx);
    pollfds_.pop_back();
  }
}
}
