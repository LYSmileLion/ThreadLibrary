#ifndef FRAMEWORK_INCLUDE_LOGSTREAM_HPP_
#define FRAMEWORK_INCLUDE_LOGSTREAM_HPP_

#include <assert.h>
#include <string>

#include <string.h>

#include <nocopyable.hpp>
#include <FixBuffer.hpp>

namespace Log {

class LogStream : public Base::nocopyable {

 public:
    typedef LogStream self;

    typedef FixedBuffer<kSmallBuffer> Buffer;

    self& operator<< (bool v) {
      buffer_.append(v ? "true " : "false", 5);
      return *this;
    }

    self& operator<<(short);

    self& operator<<(unsigned short);

    self& operator<<(int);

    self& operator<<(unsigned int);

    self& operator<<(long);

    self& operator<<(unsigned long);

    self& operator<<(long long);

    self& operator<<(unsigned long long);

    self& operator<<(const void*);

    self& operator<<(float v);

    self& operator<<(double);

    self& operator<<(char v) ;

    self& operator<<(const char* str);

    self& operator<<(const unsigned char* str);

    self& operator<<(const std::string v);

    void append(const char* data, int len);

    const Buffer& getBuffer() const;

    void resetBuffer();
 private:
  void staticCheck();

  template<typename T> void formatInteger(T);

  Buffer buffer_;

  static const int kMaxNumericSize = 32;
};
/*
class Fmt // : boost::noncopyable
{
 public:
  template<typename T>
  Fmt(const char* fmt, T val);

  const char* data() const { return buf_; }

  int length() const { return length_; }

 private:
  char buf_[32];

  int length_;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
  s.append(fmt.data(), fmt.length());
  return s;
}*/

}
#endif  // MUDUO_BASE_LOGSTREAM_H

