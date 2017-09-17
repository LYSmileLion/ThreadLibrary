#include <Thread.hpp>

#include <CurrentThread.hpp>

#include <string>
#include <iostream>

void thread() {
	std::cout << "running thread" << std::endl;
	std::cout << HPCs::CurrentThread::getName() << std::endl;;
}

int main() {
	std::string str("thread 1");
	HPCs::Thread thr1(thread,str);
	thr1.start();
	thr1.join();
}
