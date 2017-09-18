#include <time.h>
#include <pwd.h>
#include <unistd.h>

#include <iostream>

#include <LogFile.hpp>

using namespace HPCs {
LogFile::LogFile(const string& basename, size_t rollSize, bool threadSafe, int flushInterval, int checkEveryN)
    : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    checkEveryN_(checkEveryN),
    count_(0),
    mutex_(threadSafe ? new MutexLock : NULL),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0) {
  assert(basename.find('/') == string::npos);
  rollFile();
}

LogFile::~LogFile() {}

void LogFile::append(const char* logline, int len) {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
  } else {
    append_unlocked(logline, len);
  }
}

void LogFile::flush() {
  if (mutex_) {
    MutexLockGuard lock(*mutex_);
    file_->flush();
  } else {
    file_->flush();
  }
}

void LogFile::append_unlocked(const char* logline, int len) {
  file_->append(logline, len);

  if (file_->writtenBytes() > rollSize_) {
    rollFile();
  } else {
    ++count_;
    if (count_ >= checkEveryN_) {//如果写入的字节小于roolsize，则每写checkEveryN_个字节便检查一下是否需要另起文件进行写入
      count_ = 0;
      time_t now = ::time(NULL);
      time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
      if (thisPeriod_ != startOfPeriod_) {
        rollFile();
      }
      else if (now - lastFlush_ > flushInterval_) {
          //短时间内大量写入，则需要冲洗
        lastFlush_ = now;
        file_->flush();
      }
    }
  }
}

bool LogFile::rollFile() {
  time_t now = 0;
  std::string filename = getLogFileName(basename_, &now);
  time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

  if (now > lastRoll_)
  {
    lastRoll_ = now;
    lastFlush_ = now;
    startOfPeriod_ = start;
    file_.reset(new FileUtil::AppendFile(filename));
    return true;
  }
  return false;
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
  string filename;
  filename.reserve(basename.size() + 124);
  filename = basename;

  char timebuf[32];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm); // 日历时间timep转换为用UTC时间表示的时间保存在tm中
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;

  char hostname[30];
  gethostname(hostname, 30);
  filename += hostname;

  char username[30];
  struct passwd *pwd;
  pwd = getpwuid(getuid());
  filename += pwd->pw_name;

  char pidbuf[32];
  pid_t pid = getpid();
  snprintf(pidbuf, sizeof pidbuf, ".%d", pid);
  filename += pidbuf;

  filename += ".log";

  return filename;
}


}
