#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include <Logging.hpp>

#include <CurrentThread.hpp>

using namespace HPCs;

Logger::Logger(SourceFile file, int line)
  : level_(INFO), basename_(file), line_(line) {}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : level_(level),  basename_(file), line_(line) {}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
  : level_(level), basename_(file), line_(line) {
  stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort)
  : level_(toAbort?FATAL:ERROR), basename_(file), line_(line) {}

Logger::~Logger() {
  finish();
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
  if (level_ == FATAL) {
    g_flush();
    abort();
  }
}

void Logger::setLogLevel(Logger::LogLevel level) {
  g_logLevel = level;
}

void Logger::setOutput(OutputFunc out) {
  g_output = out;
}

void Logger::setFlush(FlushFunc flush) {
  g_flush = flush;
}
