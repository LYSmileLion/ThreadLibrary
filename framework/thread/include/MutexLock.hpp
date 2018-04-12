#ifndef _INCLUDE_MUTEXLOCK_HPP_
#define _INCLUDE_MUTEXLOCK_HPP_

#include <nocopyable.hpp>
#include <pthread.h>

namespace Threads {//high performance 

class MutexLock : public Base::nocopyable {
 public:
    MutexLock();

    ~MutexLock();

    bool isLockedByThisThread() const;

    void lock();

    void unlock();

    pthread_mutex_t* getPthreadMutex(); /* non-const */

 private:
	friend class Condition;

	class ResetHolderPid : nocopyable {
	 public:
		ResetHolderPid(MutexLock &owner) : owner_(owner) {
			owner_.resetHolderPid();
		}

		~ResetHolderPid() {
			owner_.assignHolderPid();
		}
	 private:
		MutexLock &owner_;
	};
    void resetHolderPid();

    void assignHolderPid();

    pid_t holder_pid_;

    pthread_mutex_t mutex_;
};

class MutexLockGuard : public Base::nocopyable {
 public:
  explicit MutexLockGuard(MutexLock& mutex)
    : mutex_(mutex) {
    mutex_.lock();
  }

  ~MutexLockGuard() {
    mutex_.unlock();
  }

 private:

  MutexLock& mutex_;
};

}
#endif
