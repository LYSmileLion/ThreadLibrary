#ifndef FRAMEWORK_INCLUDE_LOGGING_HPP_
#define FRAMEWORK_INCLUDE_LOGGING_HPP_

#include <functional>

#include <LogStream.hpp>
#include <nocopyable.hpp>

namespace Log {

enum LogLevel {
    TRACE           = 0,
    DEBUG           = 1,
    INFO            = 2,
    WARN            = 3,
    ERROR           = 4,
    FATAL           = 5,
    NUM_LOG_LEVELS  = 6,
};

class SourceFile {
 public:
    template<int N>
    inline SourceFile(const char (&arr)[N]) : data_(arr), size_(N-1) {
        const char* slash = strrchr(data_, '/'); // builtin function
        if (slash) {
            data_ = slash + 1;
            size_ -= static_cast<int>(data_ - arr);
        }
    }

    explicit SourceFile(const char* filename) : data_(filename) {
        const char* slash = strrchr(filename, '/');
        if (slash) {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;

    int size_;
};

class Logger {
 public:
    typedef std::function<void(const char*, int)> OutputFunc;

    typedef std::function<void()> FlushFunc;

    Logger(SourceFile file, int line);

    Logger(SourceFile file, int line, LogLevel level);

    Logger(SourceFile file, int line, LogLevel level, const char* func);

    Logger(SourceFile file, int line, bool toAbort);

    ~Logger();

    LogStream& stream() { return stream_; }

    static LogLevel logLevel();

    static void setLogLevel(LogLevel level);

    static void setOutput(OutputFunc);

    static void setFlush(FlushFunc);
 private:
    bool FormatTime();

    void finish();

 private:
    static OutputFunc output_func_;

    static FlushFunc flush_func_;

    LogStream stream_;

    LogLevel level_;

    SourceFile basename_;

    int line_;

    bool print_errno_;
};
}

#define LOG_TRACE if (Log::Logger::logLevel() <= Log::LogLevel::TRACE) \
    Log::Logger(__FILE__, __LINE__, Log::LogLevel::TRACE, __func__).stream()

#define LOG_DEBUG if (Log::Logger::logLevel() <= Log::LogLevel::DEBUG) \
    Log::Logger(__FILE__, __LINE__, Log::LogLevel::DEBUG, __func__).stream()

#define LOG_INFO if (Log::Logger::logLevel() <= Log::LogLevel::INFO) \
    Log::Logger(__FILE__, __LINE__).stream()

#define LOG_WARN Log::Logger(__FILE__, __LINE__, Log::LogLevel::WARN).stream()

#define LOG_ERROR Log::Logger(__FILE__, __LINE__, Log::LogLevel::ERROR).stream()

#define LOG_FATAL Log::Logger(__FILE__, __LINE__, Log::LogLevel::FATAL).stream()

#define LOG_SYSERR Log::Logger(__FILE__, __LINE__, false).stream()

#define LOG_SYSFATAL Log::Logger(__FILE__, __LINE__, true).stream()

#endif  // MUDUO_BASE_LOGGING_H
