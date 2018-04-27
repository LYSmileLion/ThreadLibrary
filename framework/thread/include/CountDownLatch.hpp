#ifndef FRAMEWORK_INCLUDE_COUNTDOWNLATCH_HPP_
#define FRAMEWORK_INCLUDE_COUNTDOWNLATCH_HPP_

#include <Condition.hpp>
#include <MutexLock.hpp>
#include <nocopyable.hpp>

namespace Threads {

class CountDownLatch : Base::nocopyable {
 public:
    explicit CountDownLatch(int count);

    void Wait();

    void CountDown();

    int GetCount();

 private:
    MutexLock mutex_;

    Condition condition_;

    int count_;
};

}
#endif
