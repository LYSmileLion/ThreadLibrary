#include <Condition.hpp>
#include <utils.hpp>

using namespace HPCs;

Condition::Condition(MutexLock &mutex) : mutex_(mutex) {
    CHECKERROR(pthread_cond_init(&pcond_, NULL));
}

Condition::~Condition() {
    CHECKERROR(pthread_cond_destroy(&condition_));
}
  
void wait() {
    MutexLock::UnassignGuard ug(mutex_);
    CHECKERROR(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
}
