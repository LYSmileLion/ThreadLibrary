#include <time.h>
#include <Condition.hpp>
#include <utils.hpp>
#include <errno.h>

namespace Threads {

Condition::Condition(MutexLock &mutex) : mutex_(mutex) {
    CHECKERROR(pthread_cond_init(&condition_, NULL));
}

Condition::~Condition() {
    CHECKERROR(pthread_cond_destroy(&condition_));
}
  
void Condition::wait() {
    MutexLock::ResetHolderPid resetholdpid(mutex_);
    CHECKERROR(pthread_cond_wait(&condition_, mutex_.getPthreadMutex()));
}

bool Condition::waitForSeconds(double seconds) {

  struct timespec abstime;
  clock_gettime(CLOCK_REALTIME, &abstime);

  const int64_t kNanoSecondsPerSecond = 1e9;
  int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

  abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
  abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

  MutexLock::ResetHolderPid resetholdpid(mutex_);
  return ETIMEDOUT == pthread_cond_timedwait(&condition_, mutex_.getPthreadMutex(), &abstime);
}

void Condition::notify() {
	CHECKERROR(pthread_cond_signal(&condition_));
}

void Condition::notifyAll() {
	CHECKERROR(pthread_cond_broadcast(&condition_));
}
}
