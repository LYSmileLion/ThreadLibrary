#ifndef INCLUDE_BOUNDEDBLOCKINGQUEUE_HPP_
#define INCLUDE_BOUNDEDBLOCKINGQUEUE_HPP_

#include <Condition.hpp>
#include <utils.hpp>
#include <nocopyable.hpp>
#include <queue>

namespace Threads {

template<typename T>
class BoundedBlockingQueue : Base::noncopyable
{
 public:
	explicit BoundedBlockingQueue(int maxSize) : 
		mutex_(),
		notEmpty_(mutex_),
	    notFull_(mutex_),
		queue_(maxSize) {}

	void put(const T& x) {
		MutexLockGuard lock(mutex_);
		while (queue_.size() >= max_size_) {
			notFull_.wait();
		}
		queue_.push_back(x);
		notEmpty_.notify();
	}

	T take() {
		MutexLockGuard lock(mutex_);
		while (queue_.empty()) {
			notEmpty_.wait();
		}
		T front(queue_.front());
		queue_.pop_front();
		notFull_.notify();
		return front;
	}

	bool empty() const {
		MutexLockGuard lock(mutex_);
		return queue_.empty();
	}

	bool full() const {
		MutexLockGuard lock(mutex_);
		return queue_.size() >= max_size_;
	}

	size_t size() const {
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

	size_t capacity() const {
		MutexLockGuard lock(mutex_);
		return queue_.capacity();
	}

 private:
	mutable MutexLock          mutex_;
	Condition                  notEmpty_;
	Condition                  notFull_;
	std::queue<T>  queue_;
};

}

#endif
