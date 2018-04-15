#include <ThreadPool.hpp>
#include <iostream>

using namespace HPCs;


void displayA() {
	std::cout << "display a" << std::endl;

}

void displayB() {
	std::cout << "display b" << std::endl;
}

int main() {
	ThreadPool thread_pool;
	const int numtask = 4;
	thread_pool.start(numtask);
	thread_pool.runTask(displayA);
	thread_pool.runTask(displayB);
	thread_pool.runTask(displayB);
	thread_pool.runTask(displayB);
	thread_pool.runTask(displayB);
	thread_pool.runTask(displayB);
}
