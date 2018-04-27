#include <Logging.hpp>
using namespace Log;
int main() {
    LOG_TRACE << "level : trace. test log trace ";
    LOG_DEBUG << "level : trace. test log debug ";
    LOG_INFO << "level : trace. test log info ";
    LOG_WARN << "level : trace. test log warn ";
    LOG_ERROR << "level : trace. test log error ";
    //LOG_FATAL << "level : trace. test log fatal ";
    //LOG_SYSERR << "level : trace. test log syserr ";
    LOG_SYSFATAL << "level : trace. test log sysfatal ";
    return 0;
}
