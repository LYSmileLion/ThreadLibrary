#ifndef FRAMEWORK_INCLUDE_LOGGING_HPP_
#define FRAMEWORK_INCLUDE_LOGGING_HPP_

#include <functional>

#include <LogStream.h>

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
	inline SourceFile(const char (&arr)[N])
	    : data_(arr), size_(N-1)
	{
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
    
    OutputFunc output_func_;

    FlushFunc flush_func_;

	LogStream stream_;
  
	LogLevel level_;
  
	int line_;
  
	SourceFile basename_;
};

extern LogLevel g_logLevel;

inline LogLevel Logger::logLevel()
{
  return g_logLevel;
}

#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) \
	Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()

#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
	Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()

#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
	Logger(__FILE__, __LINE__).stream()

#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()

#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()

#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()

#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()

#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()
}

#endif  // MUDUO_BASE_LOGGING_H
