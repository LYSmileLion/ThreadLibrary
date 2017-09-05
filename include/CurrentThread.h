#ifndef INCLUDE_CURRENTTHREAD_HPP_
#define INCLUDE_CURRENTTHREAD_HPP

#include <stdint.h>

namespace HPCs {

namespace CurrentThread {

  extern __thread int t_cache_tid;
  extern __thread char t_tid_string[32];
  extern __thread int t_tid_string_length;
  extern __thread const char* t_thread_name;

  void CacheTid();

  inline int GetTid() {
    if (t_cachedTid == 0) {
      CacheTid();
    }
    return t_cached_tid;
  }

  inline const char* GetTidString() {
    return t_tid_string;
  }

  inline int GetTidStringLength() {
    return t_tid_string_length;
  }

  inline const char* GetName() {
    return t_thread_name;
  }

  bool IsMainThread();

  void SleepUsec(int64_t usec);
}
}

#endif
