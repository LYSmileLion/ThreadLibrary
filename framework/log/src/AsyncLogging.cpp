#include <AsyncLogging.hpp>
#include <LogFile.hpp>

#include <stdio.h>
#include <assert.h>

#include <iostream>

namespace Log {

AsyncLogging::AsyncLogging(
    const std::string& basename,
    long int rollSize,
    int flushInterval) :
    flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogging::ThreadFunc, this), "AsyncLogging"),
    latch_(1),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_() {
    currentBuffer_->fillZero();
    nextBuffer_->fillZero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
    if (running_) {
        Stop();
    }
}

void AsyncLogging::Stop() {
    running_ = false;
    cond_.notify();
    thread_.join();
}

void AsyncLogging::Start() {
    running_ = true;
    thread_.start();
    latch_.Wait();
}

void AsyncLogging::Append(const char* logline, int len) {
    Threads::MutexLockGuard lock(mutex_);
    if (currentBuffer_->getLeftBufferSize() > len) {
        currentBuffer_->append(logline, len);
    } else {
        buffers_.push_back(std::move(currentBuffer_));
        if (nextBuffer_) {
            currentBuffer_ = std::move(nextBuffer_);
        } else {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::ThreadFunc() {
    assert(running_ == true);
    latch_.CountDown();
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->fillZero();
    newBuffer2->fillZero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_) {
        {
            Threads::MutexLockGuard lock(mutex_);
            if (buffers_.empty()) {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        if (buffersToWrite.size() > 25) {
            char buf[256];
            snprintf(
                buf, 
                sizeof(buf), 
                "Dropped log messages , %zd larger buffers\n",
            buffersToWrite.size()-2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(
            buffersToWrite.begin() + 2,
            buffersToWrite.end());
        }
        std::cout << "size : " << buffersToWrite.size() << std::endl;
        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
            output.append(
                buffersToWrite[i]->getDataPtr(), 
                buffersToWrite[i]->getLength());
        }

        if (buffersToWrite.size() > 2) {
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
  }
  output.flush();
}

}
