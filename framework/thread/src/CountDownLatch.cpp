#include <CountDownLatch.hpp>

namespace Threads {
CountDownLatch::CountDownLatch(int count) :
    mutex_(),
    condition_(mutex_),
    count_(count) {}

void CountDownLatch::Wait() {
    MutexLockGuard lock(mutex_);
    while (count_ > 0) {
        condition_.wait();
    }
}

void CountDownLatch::CountDown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0) {
        condition_.notifyAll();
    }
}

int CountDownLatch::GetCount() {
    MutexLockGuard lock(mutex_);
    return count_;
}

}
