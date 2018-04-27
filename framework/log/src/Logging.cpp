#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sstream>
#include <string>

#include <Logging.hpp>

#include <CurrentThread.hpp>

namespace Log {

void DefaultOutput(const char *msg, int len) {
    ::fwrite(msg, 1, len, stdout);
}

void DefaultOutput() {
    fflush(stdout)    
}

OutputFunc Logger::output_func_ = DefaultOutput;
FlushFunc Logger::flush_func_ = DefaultOutput;

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
}

Logger::Logger(SourceFile file, int line) : 
    level_(INFO), basename_(file), line_(line) {
    FormatTime();
    Threads::CurrentThread::getTid();
    stream_ << Threads::CurrentThread::getTidString();
    stream_ << LogLevelName[level];
}

Logger::Logger(SourceFile file, int line, LogLevel level) : 
    level_(level),  basename_(file), line_(line) {}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func) : 
    level_(level), basename_(file), line_(line) {
  stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort) : 
    level_(toAbort?FATAL:ERROR), basename_(file), line_(line) {}

Logger::FormatTime() {
    struct timeval time = {0, 0};
    ::gettimeofday(&time);
    struct tm ;
    memset(&tm, 0, sizeof(tm));
    ::localtime_r(static_cast<time_t *>(time.tv_sec), &tm);
    long int msec = time.tv_usec / 1000;
    char timebuf[32];
    ::strftime(timebuf, sizeof timebuf, "%Y%m%d-%H%M%S.", &tm);
    std::string time_str = std::string(timebuf) + std::to_string(msec);
    stream_ << time_str.str();
}

Logger::~Logger() {
    finish();
    const LogStream::Buffer& buf(stream().buffer());
    output_func_(buf.data(), buf.length());
    if (level_ == FATAL) {
      g_flush();
      abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level) {
  g_logLevel = level;
}

void Logger::setOutput(OutputFunc out) {
  output_func_ = out;
}

void Logger::setFlush(FlushFunc flush) {
  flush_func_ = flush;
}
}
