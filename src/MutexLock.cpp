#include <MutexLock.hpp>

using namespace HPCs;

MutexLock::MutexLock() : holder_pid_(0) {
    CHECKERROR(pthread_mutex_init(&mutex_, NULL));
}

MutexLock::~MutexLock() {
    CHECKERROR(pthread_mutex_destroy(&mutex_));
}

bool MutexLock::isLockedByThisThread() const {
    return holder_pid_ == HPCs::CurrentThread::GetTid();
}

void MutexLock::lock() {
    CHECKERROR(pthread_mutex_lock(&mutex_));
    assignHolder();
}

void MutexLock::unlock() {
    unassignHolder();
    CHECKERROR(pthread_mutex_unlock(&mutex_));
}

pthread_mutex_t* MutexLock::getPthreadMutex() {
   return &mutex_;
}

void MutexLock::resetHolderPid() {
    holder_pid_ = 0;
}

void MutexLock::getHolderPid() {
    holder_pid_ = HPCs::CurrentThread::GetTid();
}
