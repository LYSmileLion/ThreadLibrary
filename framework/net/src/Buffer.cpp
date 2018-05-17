#include <errno.h>
#include <sys/uio.h>

#include <string>

#include <Buffer.hpp>
#include <Socket.hpp>
#include <Endian.hpp>
#include <Logging.hpp>

namespace Net {

const char Buffer::CRLF_[] = "\r\n";

const size_t Buffer::cheap_prepend_ = 8;

const size_t Buffer::initial_size_ = 1024;

Buffer::Buffer(size_t initialSize) :
    buffer_(cheap_prepend_ + initial_size_),
    reader_index_(cheap_prepend_),
    writer_index_(cheap_prepend_) {}

size_t Buffer::ReadableBytes() const {
    return writer_index_ - reader_index_;
}

size_t Buffer::WritableBytes() const {
    return buffer_.size() - writer_index_;
}

size_t Buffer::PrependableBytes() const {
    return reader_index_;
}

const char* Buffer::Peek() const {
    return Begin() + reader_index_;
}

const char* Buffer::FindEOL() const {
    const void* eol = memchr(Peek(), '\n', ReadableBytes());
    return static_cast<const char*>(eol);
}

const char* Buffer::FindEOL(const char* start) const {
    const char *start_search = start;
    if (start < Peek()) {
        start_search = Peek();
    }
    if (start > BeginWrite()) {
        return NULL;
    }
    const void* eol = memchr(start_search, '\n', BeginWrite() - start_search);
    return static_cast<const char*>(eol);
}

const char* Buffer::FindCRLF() const {
    const char* crlf = std::search(Peek(), BeginWrite(), CRLF_, CRLF_ + 2);
    return crlf == BeginWrite() ? NULL : crlf;
}

const char* Buffer::FindCRLF(const char* start) const {
    const char *start_search = start;
    if (start < Peek()) {
        start_search = Peek();
    }
    if (start > BeginWrite()) {
        return NULL;
    }
    const char* crlf = std::search(start_search, BeginWrite(), CRLF_, CRLF_ + 2);
    return crlf == BeginWrite() ? NULL : crlf;
}

void Buffer::Retrieve(size_t len) {
    if (len < ReadableBytes()) {
      reader_index_ += len;
    } else {
      RetrieveAll();
    }
}

void Buffer::RetrieveUntil(const char* end) {
    if (end <= Peek()) {
        return;
    }
    if (end >= BeginWrite()) {
        RetrieveAll();
    }
    Retrieve(end - Peek());
}

void Buffer::RetrieveInt64() {
    Retrieve(sizeof(int64_t));
}

void Buffer::RetrieveInt32() {
    Retrieve(sizeof(int32_t));
}

void Buffer::RetrieveInt16() {
    Retrieve(sizeof(int16_t));
}

void Buffer::RetrieveInt8() {
    Retrieve(sizeof(int8_t));
}

void Buffer::RetrieveAll() {
    reader_index_ = cheap_prepend_;
    writer_index_ = cheap_prepend_;
}

std::string Buffer::RetrieveAllAsString() {
    return RetrieveAsString(ReadableBytes());
}

std::string Buffer::RetrieveAsString(size_t len) {
    size_t read_len = len;
    if (len >= ReadableBytes()) {
        read_len = ReadableBytes();
    }
    std::string result(Peek(), read_len);
    Retrieve(len);
    return result;
}

void Buffer::Append(const char* data, size_t len) {
    EnsureWritableBytes(len);
    std::copy(data, data+len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const void* data, size_t len) {
    Append(static_cast<const char*>(data), len);
}

void Buffer::EnsureWritableBytes(size_t len) {
    if (WritableBytes() < len) {
        MakeSpace(len);
    }
}

char* Buffer::BeginWrite() {
    return Begin() + writer_index_;
}

const char* Buffer::BeginWrite() const {
    return Begin() + writer_index_;
}

void Buffer::HasWritten(size_t len) {
    if (len > WritableBytes()) {
        LOG_FATAL << "not have enough space in buffer.";
    }
    writer_index_ += len;
}

void Buffer::Unwrite(size_t len) {
    if (len > ReadableBytes()) {
        LOG_FATAL << "not have enough space in readable space.";
    }
    writer_index_ -= len;
}

void Buffer::AppendInt64(int64_t num) {
    int64_t value = HostToNetwork64(num);
    Append(&value, sizeof(value));
}

void Buffer::AppendInt32(int32_t num) {
    int32_t value = HostToNetwork32(num);
    Append(&value, sizeof(value));
}

void Buffer::AppendInt16(int16_t num) {
    int16_t value = HostToNetwork16(num);
    Append(&value, sizeof(value));
}

void Buffer::AppendInt8(int8_t num) {
    Append(&num, sizeof(num));
}

int64_t Buffer::ReadInt64() {
    int64_t result = PeekInt64();
    RetrieveInt64();
    return result;
}

int32_t Buffer::ReadInt32() {
    int32_t result = PeekInt32();
    RetrieveInt32();
    return result;
}

int16_t Buffer::ReadInt16() {
    int16_t result = PeekInt16();
    RetrieveInt16();
    return result;
}

int8_t Buffer::ReadInt8() {
    int8_t result = PeekInt8();
    RetrieveInt8();
    return result;
}

int64_t Buffer::PeekInt64() const {
    if (ReadableBytes() < sizeof(int64_t)) {
        LOG_FATAL << "have not enough data for read.";
    }
    int64_t value = 0;
    ::memcpy(&value, Peek(), sizeof(value));
    return NetworkToHost64(value);
}

int32_t Buffer::PeekInt32() const {
    if (ReadableBytes() < sizeof(int32_t)) {
        LOG_FATAL << "have not enough data for read.";
    }
    int32_t value = 0;
    ::memcpy(&value, Peek(), sizeof(value));
    return NetworkToHost32(value);
}

int16_t Buffer::PeekInt16() const {
    if (ReadableBytes() < sizeof(int16_t)) {
        LOG_FATAL << "have not enough data for read.";
    }
    int16_t value = 0;
    ::memcpy(&value, Peek(), sizeof(value));
    return NetworkToHost16(value);
}

int8_t Buffer::PeekInt8() const {
    if (ReadableBytes() < sizeof(int8_t)) {
        LOG_FATAL << "have not enough data for read.";
    }
    int8_t value = *Peek();
    return value;
}

void Buffer::PrependInt64(int64_t num) {
    int64_t value = HostToNetwork64(num);
    Prepend(&value, sizeof(value));
}

void Buffer::PrependInt32(int32_t num) {
    int32_t value = HostToNetwork32(num);
    Prepend(&value, sizeof(value));
}

void Buffer::PrependInt16(int16_t num) {
    int16_t value = HostToNetwork16(num);
    Prepend(&value, sizeof(value));
}

void Buffer::PrependInt8(int8_t num) {
    Prepend(&num, sizeof(num));
}

void Buffer::Prepend(const void* data, size_t len) {
    if (PrependableBytes() < len) {
        LOG_FATAL << "have not enough data for prepend.";
    }
    reader_index_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d+len, Begin() + reader_index_);
}

size_t Buffer::InternalCapacity() const {
    return buffer_.capacity();
}

char* Buffer::Begin() {
    return &*buffer_.begin();
}

const char* Buffer::Begin() const { 
    return &*buffer_.begin();
}

void Buffer::MakeSpace(size_t len) {
    if (WritableBytes() + PrependableBytes() < len + cheap_prepend_) {
      buffer_.resize(writer_index_ + len);
    } else {
      assert(cheap_prepend_ < reader_index_);
      size_t readable = ReadableBytes();
      std::copy(
        Begin() + reader_index_,
        Begin() + writer_index_,
        Begin() + cheap_prepend_);
      reader_index_ = cheap_prepend_;
      writer_index_ = reader_index_ + readable;
      assert(readable == ReadableBytes());
    }
}

ssize_t Buffer::ReadFd(int fd, int* savedErrno) {
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = WritableBytes();
  vec[0].iov_base = Begin()+writer_index_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
  const ssize_t n = ::readv(fd, vec, iovcnt);
  if (n < 0) {
    *savedErrno = errno;
  }
  else if (static_cast<size_t>(n) <= writable) {
    writer_index_ += n;
  }
  else {
    writer_index_ = buffer_.size();
    Append(extrabuf, n - writable);
  }
  return n;
}
}
