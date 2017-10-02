#include <Condition.hpp>
#include <Thread.hpp>
#include <MutexLock.hpp>
#include <queue>
#include <sys/time.h>
#include <iostream>

#include <unistd.h>
using namespace HPCs;

MutexLock mutex;
Condition notEmpty(mutex);

std::queue<int> que;

void displayA() {
	while(1) {
		MutexLockGuard locker(mutex);
		while(que.size() == 0) {
			notEmpty.wait();
		}
		std::cout << que.front() << std::endl;
		que.pop();
	}
}

void displayB() {
	while(1) {
		{
			MutexLockGuard locker(mutex);
			que.push(2);
			notEmpty.notify();
		}
		sleep(1);

	}

}
int main() {
	Thread thread1(displayA);
	Thread thread2(displayB);

	thread1.start();
	thread2.start();

	thread1.join();
	thread2.join();


}
