#ifndef FRAMEWORK_NET_INCLUDE_BUFFER_HPP_
#define FRAMEWORK_NET_INCLUDE_BUFFER_HPP_

#include <nocopyable.hpp>
#include <Types.hpp>

#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>

namespace Net {

class Buffer : public Base::nocopyable {
 public:
    explicit Buffer(size_t initialSize = initial_size_);

    size_t ReadableBytes() const;

    size_t WritableBytes() const;

    size_t PrependableBytes() const;

    const char* Peek() const;

    const char* FindCRLF() const;

    const char* FindCRLF(const char* start) const;

    const char* FindEOL() const;

    const char* FindEOL(const char* start) const;

    void Retrieve(size_t len);

    void RetrieveUntil(const char* end);

    void RetrieveInt64();

    void RetrieveInt32();

    void RetrieveInt16();

    void RetrieveInt8();

    void RetrieveAll();

    std::string RetrieveAllAsString();

    std::string RetrieveAsString(size_t len);

    void Append(const char* data, size_t len);

    void Append(const void* data, size_t len);

    void EnsureWritableBytes(size_t len);

    char* BeginWrite();

    const char* BeginWrite() const;

    void HasWritten(size_t len);

    void Unwrite(size_t len);

    void AppendInt64(int64_t num);

    void AppendInt32(int32_t num);

    void AppendInt16(int16_t num);

    void AppendInt8(int8_t num);

    int64_t ReadInt64();

    int32_t ReadInt32();

    int16_t ReadInt16();

    int8_t ReadInt8();

    int64_t PeekInt64() const;

    int32_t PeekInt32() const;

    int16_t PeekInt16() const;

    int8_t PeekInt8() const;

    void PrependInt64(int64_t num);

    void PrependInt32(int32_t num);

    void PrependInt16(int16_t num);

    void PrependInt8(int8_t num);

    void Prepend(const void* data, size_t len);

    size_t InternalCapacity() const;

    ssize_t ReadFd(int fd, int* savedErrno);

 private:
    char* Begin();

    const char* Begin() const;

    void MakeSpace(size_t len);

 private:
    std::vector<char> buffer_;

    size_t reader_index_;

    size_t writer_index_;

    static const char CRLF_[];

    static const size_t cheap_prepend_;

    static const size_t initial_size_;
};

}

#endif
