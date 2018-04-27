#ifndef FRAMEWORK_INCLUDE_ASYNCLOGGING_HPP_
#define FRAMEWORK_INCLUDE_ASYNCLOGGING_HPP_

#include <atomic>
#include <vector>
#include <string>

#include <Thread.hpp>
#include <FixBuffer.hpp>
#include <MutexLock.hpp>
#include <Condition.hpp>
#include <CountDownLatch.hpp>
#include <nocopyable.hpp>

namespace Log {

class AsyncLogging : Base::nocopyable {
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;
 public:
    AsyncLogging(
        const std::string& basename,
        long int rollSize,
        int flushInterval = 3);

    ~AsyncLogging();

    void Append(const char* logline, int len);

    void Start();

    void Stop();

 private:
    void ThreadFunc();

    const int flushInterval_;

    std::atomic<bool> running_;

    std::string basename_;

    long int rollSize_;

    Threads::Thread thread_;

    Threads::CountDownLatch latch_;

    Threads::MutexLock mutex_;

    Threads::Condition cond_;

    BufferPtr currentBuffer_;

    BufferPtr nextBuffer_;

    BufferVector buffers_;
};
}

#endif
