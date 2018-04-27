#include <iostream>
#include <string>
#include <LogFile.hpp>
#include <Logging.hpp>
#include <vector>
using namespace Log;

std::unique_ptr<LogFile> g_logfile;

void OutputFunc(const char *msg, int len) {
    g_logfile->append(msg, len);
}

void FlushFunc() {
    g_logfile->flush();
}

int main(){
    g_logfile.reset(new LogFile("./ssaaa", 100000));
    Logger::setOutput(OutputFunc);
    Logger::setFlush(FlushFunc);
    char *msg = "llllllllllllliuyi sdasds";
    for (int i = 0; i < 10000; i++) {
        LOG_INFO << msg << i;
    }
    return 0;
}
