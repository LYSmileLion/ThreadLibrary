#include <MutexLock.hpp>
#include <utils.hpp>
#include <Thread.hpp>
#include <CurrentThread.hpp>

namespace Threads {

MutexLock::MutexLock() : holder_pid_(0) {
   CHECKERROR(pthread_mutex_init(&mutex_, NULL));
}

MutexLock::~MutexLock() {
    CHECKERROR(pthread_mutex_destroy(&mutex_));
}

bool MutexLock::isLockedByThisThread() const {
    return holder_pid_ == CurrentThread::getTid();
}

void MutexLock::lock() {
    CHECKERROR(pthread_mutex_lock(&mutex_));
    assignHolderPid();
}

void MutexLock::unlock() {
    resetHolderPid();
    CHECKERROR(pthread_mutex_unlock(&mutex_));
}

pthread_mutex_t* MutexLock::getPthreadMutex() {
   return &mutex_;
}

void MutexLock::resetHolderPid() {
    holder_pid_ = 0;
}

void MutexLock::assignHolderPid() {
    holder_pid_ = CurrentThread::getTid();
}

}
