#ifndef _INCLUDE_FIXBUFFER_HPP_
#define _INCLUDE_FIXBUFFER_HPP_

#include <string.h>

#include <string>
#include <nocopyable.hpp>

namespace Base {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int SIZE>
class FixedBuffer : public nocopyable {
 public:
    FixedBuffer()
      : pcurrent_(data_) {
      setCookie(cookieStart);
    }

    ~FixedBuffer() {
      setCookie(cookieEnd);
    }

    void append(const char* buffer, int length) {
      if (static_cast<int>(getLeftBufferSize()) > length) {
        memcpy(pcurrent_, buffer, length);
        pcurrent_ += length;
      }
    }

    const char* getDataPtr() const { return data_; }

    int getLength() const { return static_cast<int>(pcurrent_ - data_); }

    char* getCurrentPtr() { return pcurrent_; }

    int getLeftBufferSize() const { return static_cast<int>(getEndPtr() - pcurrent_); }

    void addCurrentPtr(int len) { pcurrent_ += len; }

    void reset() { pcurrent_ = data_; }

    void fillZero() { memset(data_, 0, sizeof(data_)); }

    void setCookie(void (*cookie)()) { cookie_ = cookie; } //for gdb

    std::string toString() const { return std::string(data_, getLength()); }
 private:
    const char* getEndPtr() const { return data_ + sizeof(data_); }

    static void cookieStart();

    static void cookieEnd();

    void (*cookie_)();

    char data_[SIZE];

    char* pcurrent_;
};
}
#endif
