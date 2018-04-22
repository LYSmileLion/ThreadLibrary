#ifndef INCLUDE_CURRENTTHREAD_HPP_
#define INCLUDE_CURRENTTHREAD_HPP_

#include <stdint.h>

namespace HPCs {

namespace CurrentThread {

  extern __thread int t_cached_tid;
  extern __thread char t_tid_string[32];
  extern __thread int t_tid_string_length;
  extern __thread const char* t_thread_name;

  void cacheTid();

  inline int getTid() {
    if (t_cached_tid == 0) {
      cacheTid();
    }
    return t_cached_tid;
  }

  inline const char* getTidString() {
    return t_tid_string;
  }

  inline int getTidStringLength() {
    return t_tid_string_length;
  }

  inline const char* getName() {
    return t_thread_name;
  }

  bool isMainThread();

  void sleepUsec(int64_t usec);
}
}

#endif
