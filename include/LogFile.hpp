#ifndef _INCLUDE_LOGFILE_HPP_
#define _INCLUDE_LOGFILE_HPP_

#include <memory>
#include <string>

#include <FileUtils.hpp>
#include <MutexLock.hpp>

namespace HPCs {

class WriteFile;

class LogFile : public nocopyable{
 public:
    LogFile(
        const std::string& basename,
        uint64_t rollSize,
        bool threadSafe = true,
        int flushInterval = 3,
        int checkEveryN = 1024);

    ~LogFile();

    void append(const char* logline, int len);

    void flush();

    bool rollFile();
 private:
    void append_unlocked(const char* logline, int len);

    static std::string getLogFileName(const std::string& basename, time_t* now);

    const std::string basename_;

    const size_t rollSize_;

    const int flushInterval_;

    const int checkEveryN_;

    int count_;

    std::unique_ptr<HPCs::MutexLock> mutex_;

    time_t startOfPeriod_;

    time_t lastRoll_;

    time_t lastFlush_;

    std::unique_ptr<HPCs::WriteFile> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;

};
}

#endif
