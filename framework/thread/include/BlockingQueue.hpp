#ifndef INCLUDE_BLOCKINGQUEUE_HPP_
#define INCLUDE_BLOCKINGQUEUE_HPP_

namespace Threads {

template<typename T>
class BlockingQueue : public Bass::noncopyable {
 public:
	BlockingQueue() : mutex_(),  notEmpty_(mutex_), queue_() {}

	void put(const T& x) {
		 MutexLockGuard lock(mutex_);
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
		 return front;
	 }

	 size_t size() const {
		 MutexLockGuard lock(mutex_);
		 return queue_.size();
	 }

 private:
	 mutable MutexLock mutex_;
	 Condition         notEmpty_;
	 std::deque<T>     queue_;
};
}
#endif
