#ifdef  _INCLUDE_THREAD_HPP_
#define _INCLUDE_THREAD_HPP_

#include <functional>
#include <memory>
#include <string>
#include <nocopyable.hpp>
#include <pthread.h>

namespace HPCs {//high performance 

class Thread : noncopyable
{
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(const ThreadFunc&, const string& name = string());

  ~Thread();

  void start();

  int Join();

  bool IsStarted() const;

  pid_t Tid() const;

  const string& Name() const;

 private:
  void SetDefaultName();

  bool started_;

  bool joined_;

  pthread_t pthreadId_;

  std::shared_ptr<pid_t> tid_;

  ThreadFunc func_;

  string name_;
};

}

#endif
