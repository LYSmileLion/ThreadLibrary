#ifndef _INCLUDE_LOGGING_HPP_
#define _INCLUDE_LOGGING_HPP_

#include <LogStream.h>

namespace HPCs {

enum LogLevel {
	TRACE,
	DEBUG,INFO,
	WARN,
	ERROR,
	FATAL,
	NUM_LOG_LEVELS,
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
	typedef void (*OutputFunc)(const char* msg, int len);
  
	typedef void (*FlushFunc)();
	
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
    OutputFunc g_output;

    FlushFunc g_flush;

	void finish();
	
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
