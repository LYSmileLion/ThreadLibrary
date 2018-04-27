#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <sstream>
#include <string>
#include <iostream>

#include <ColorControl.hpp>
#include <Logging.hpp>
#include <CurrentThread.hpp>

namespace Log {

LogLevel g_logLevel = TRACE;

void DefaultOutput(const char *msg, int len) {
    ::fwrite(msg, 1, len, stdout);
}

void DefaultFlush() {
    fflush(stdout);
}

const char* LogLevelName[LogLevel::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

Logger::OutputFunc Logger::output_func_ = DefaultOutput;
Logger::FlushFunc Logger::flush_func_ = DefaultFlush;

Logger::Logger(SourceFile file, int line) :
    level_(INFO), 
    basename_(file), 
    line_(line),
    print_errno_(false) {
    FormatTime();
    Threads::CurrentThread::getTid();
    stream_ << Threads::CurrentThread::getTidString();
    stream_ << LogLevelName[level_];
}

Logger::Logger(SourceFile file, int line, LogLevel level) : 
    level_(level),
    basename_(file),
    line_(line),
    print_errno_(false) {
    FormatTime();
    Threads::CurrentThread::getTid();
    stream_ << Threads::CurrentThread::getTidString();
    stream_ << LogLevelName[level_];
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func) :
    level_(level),
    basename_(file),
    line_(line),
    print_errno_(false) {
    FormatTime();
    Threads::CurrentThread::getTid();
    stream_ << Threads::CurrentThread::getTidString();
    stream_ << LogLevelName[level_];
    stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort) : 
    level_(toAbort ? FATAL : ERROR),
    basename_(file),
    line_(line),
    print_errno_(true) {
    FormatTime();
    Threads::CurrentThread::getTid();
    stream_ << Threads::CurrentThread::getTidString();
    stream_ << LogLevelName[level_];
}

bool Logger::FormatTime() {
    // get time have no timezone
    struct timeval time = {0, 0};
    ::gettimeofday(&time, NULL);
    //get time info have timezone
    struct tm tm_info;
    memset(&tm_info, 0, sizeof(tm_info));
    time_t time_value = static_cast<time_t>(time.tv_sec);
    ::localtime_r(&time_value, &tm_info);

    //get msec
    long int msec = time.tv_usec / 1000;
    char timebuf[32];
    ::strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H:%M:%S.", &tm_info);
    std::string time_str = std::string(timebuf) + std::to_string(msec);
    stream_ << time_str.c_str();
    return true;
}

Logger::~Logger() {
    finish();
    const LogStream::Buffer& buf(stream().getBuffer());

    output_func_(buf.getDataPtr(), buf.getLength());
    if (level_ == FATAL) {
      flush_func_();
      ::abort();
    }
}

void Logger::finish() {
    if (true == print_errno_) {
        stream_ << " System Error " << errno << " : " << strerror(errno);
    }
    stream_ << " - " << basename_.data_ << ':' << line_ << '\n';
}

void Logger::setLogLevel(LogLevel level) {
  g_logLevel = level;
}

LogLevel Logger::logLevel() {
  return g_logLevel;
}

void Logger::setOutput(OutputFunc out) {
  output_func_ = out;
}

void Logger::setFlush(FlushFunc flush) {
  flush_func_ = flush;
}

}
