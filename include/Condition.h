#ifndef INCLUDE_CONDITION_HPP_
#define INCLUDE_CONDITION_HPP_

#include <MutexLock.hpp>
#include <noncopyable.hpp>
#include <pthread.h>

namespace HPCs{

class Condition : noncopyable {
 public:
    explicit Condition(MutexLock& mutex);

    ~Condition();

    void wait();

    bool waitForSeconds(double seconds);

    void notify();

    void notifyAll();
 private:
    MutexLock& mutex_;

    pthread_cond_t condition_;
};

}
#endif  // MUDUO_BASE_CONDITION_H
