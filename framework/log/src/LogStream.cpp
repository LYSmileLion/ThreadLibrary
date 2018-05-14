#include <utils.hpp>
#include <LogStream.hpp>
#include <FixBuffer.hpp>

#include <iostream>

namespace Log {

template<typename T>
void LogStream::formatInteger(T v) {
  if (buffer_.getLeftBufferSize() >= kMaxNumericSize) {
    size_t len = convert(buffer_.getCurrentPtr(), v);
    buffer_.addCurrentPtr(len);
  }
}

LogStream& LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(const void* p) {
  uintptr_t v = reinterpret_cast<uintptr_t>(p);
  if (buffer_.getLeftBufferSize() >= kMaxNumericSize)
  {
    char* buf = buffer_.getCurrentPtr();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = convertHex(buf+2, v);
    buffer_.addCurrentPtr(len+2);
  }
  return *this;
}

// FIXME: replace this with Grisu3 by Florian Loitsch.
LogStream& LogStream::operator<<(double v)
{
  if (buffer_.getLeftBufferSize() >= kMaxNumericSize)
  {
    int len = snprintf(buffer_.getCurrentPtr(), kMaxNumericSize, "%.12g", v);
    buffer_.addCurrentPtr(len);
  }
  return *this;
}

LogStream& LogStream::operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
}

LogStream& LogStream::operator<<(char v) {
    buffer_.append(&v, 1);
    return *this;
}

LogStream& LogStream::operator<<(const char* str) {
    if (str) {
        buffer_.append(str, strlen(str));
    } else {
        buffer_.append("(null)", 6);
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str) {
    return operator<<(reinterpret_cast<const char*>(str));
}

LogStream& LogStream::operator<<(const std::string v) {
    buffer_.append(v.c_str(), v.size());
    return *this;
}

void LogStream::append(const char* data, int len) { 
    buffer_.append(data, len); 
}

const LogStream::Buffer& LogStream::getBuffer() const {
    return buffer_;
}

void LogStream::resetBuffer() { 
    buffer_.reset();
}
/*template<typename T>
  Fmt::Fmt(const char* fmt, T val)
{
  BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);

  length_ = snprintf(buf_, sizeof buf_, fmt, val);
  assert(static_cast<size_t>(length_) < sizeof buf_);
}

// Explicit instantiations

template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);*/
}
