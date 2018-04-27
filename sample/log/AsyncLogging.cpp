#include <iostream>
#include <string>
#include <vector>

#include <AsyncLogging.hpp>
#include <LogFile.hpp>
#include <Logging.hpp>
#include <ThreadPool.hpp>

#include <unistd.h>

using namespace Log;
using namespace Threads;

AsyncLogging g_async_logging(std::string("asynclogging"), 1024 * 1024);

void OutputFunc(const char *msg, int len) {
    g_async_logging.Append(msg, len);
}

void FlushFunc() {
    return;
}

void OutputLog() {
    for (int i = 0; i < 20; i++) {
        LOG_INFO << "output info log";
        sleep(1);
    }
}

int main(){
    g_async_logging.Start();
    Logger::setOutput(OutputFunc);
    Logger::setFlush(FlushFunc);
    Thread thread1(OutputLog, std::string("threads1"));
    Thread thread2(OutputLog, std::string("threads2"));
    thread1.start();
    thread2.start();
    thread1.join();
    thread2.join();
    g_async_logging.Stop();
    return 0;
}
