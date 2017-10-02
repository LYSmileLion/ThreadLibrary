#include <ThreadPool.hpp>
#include <MutexLock.hpp>
#include <iostream>

using namespace HPCs;

ThreadPool::ThreadPool(const std::string threadpool_name) : 
    mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    name_(threadpool_name),
    max_queue_size_(0),
    running_(false){}

ThreadPool::~ThreadPool() {
    if (running_) {
        stop();
    }
}

void ThreadPool::start(int num_threads) {
    if (!threads_.empty()) {
        std::cout << "start failed" << std::endl;
        return;
    }

    running_ = true;
    threads_.reserve(num_threads);
    for (int i = 0; i < num_threads; i++) {
		char id[32];
		snprintf(id, sizeof(id), "%d", i + 1);
		threads_.push_back(std::shared_ptr<Thread>(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id)));
		threads_[i]->start();
    }
	if (num_threads == 0 && threadInitCallback_) {
		threadInitCallback_();
	}
}

void ThreadPool::stop() {
	{
		MutexLockGuard lock(mutex_);
		running_ == false;
		notEmpty_.notifyAll();
	}
	for_each(threads_.begin(), threads_.end(), std::bind(&Thread::join, std::placeholders::_1));
}

void ThreadPool::runInThread() {
	if (threadInitCallback_) {
		threadInitCallback_();
	}
	while (running_) {
		Task task(take());
		if (task) {
			task();
		}
	}
}

ThreadPool::Task ThreadPool::take() {
	MutexLockGuard lock(mutex_);
	while (queue_.empty() && running_) {
		notEmpty_.wait();
	}

	Task task;
	if (!queue_.empty()) {
		task = queue_.front();
		queue_.pop_front();
		if (max_queue_size_ > 0) {
			notFull_.notify();
		}
	}

	return task;
}

void ThreadPool::runTask(const Task &task) {
	if (threads_.empty()) {
		task();
	} else {
		MutexLockGuard lock(mutex_);
		while (isFull()) {
			notFull_.wait();
		}
		queue_.push_back(task);
		notEmpty_.notify();
	}
}

bool ThreadPool::isFull() const {
	return max_queue_size_ > 0 && queue_.size() >= max_queue_size_;
}
