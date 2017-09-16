#include <ThreadPool.hpp>

using namespace HPCs;

ThreadPool::ThreadPool(const std::string& threadpool_name) : 
    mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    name_(threadpool_name),
    max_queue_size(0),
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
        
    }
}
