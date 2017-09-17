#include <iostream>
#include <unistd.h>
#include <MutexLock.hpp>
#include <Thread.hpp>

HPCs::MutexLock mutex;
void thread1(){
	HPCs::MutexLockGuard lock(mutex);
	sleep(2);
	std::cout << "thread1" << std::endl;

}
void thread2(){
	HPCs::MutexLockGuard lock(mutex);
	std::cout << "thread2" << std::endl;

}

int main() {
	{
		HPCs::Thread thr1(thread1, "thread1");
		HPCs::Thread thr2(thread2, "thread2");
		thr2.start();
		thr1.start();
	}
	sleep(10);
	return 0;
}
