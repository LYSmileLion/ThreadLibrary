#ifndef _INCLUDE_THREAD_HPP_
#define _INCLUDE_THREAD_HPP_

#include <pthread.h>

#include <functional>
#include <memory>
#include <string>

#include <nocopyable.hpp>


namespace HPCs {//high performance 

class Thread : public nocopyable {
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(const ThreadFunc&, const std::string& name = std::string());

  ~Thread();

  void start();

  int join();

  bool isStarted() const;

  pid_t tid() const;

  const std::string& name() const;

 private:
  void setDefaultName();

  bool started_;

  bool joined_;

  pthread_t pthreadId_;

  std::shared_ptr<pid_t> tid_;

  ThreadFunc func_;

  std::string name_;
};

}

#endif
