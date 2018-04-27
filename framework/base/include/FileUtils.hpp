#ifndef _INCLUDE_FILEUTILS_HPP_
#define _INCLUDE_FILEUTILS_HPP_

#include <stdio.h>
#include <stdint.h>

#include <string>

#include <nocopyable.hpp>
#include <Types.hpp>

namespace Base {

class ReadFile : public nocopyable {
 public:
    explicit ReadFile(const std::string &filename);

    ~ReadFile();

    Status readToString(
        uint64_t readsize,
        int64_t *modifytime,
        int64_t *createtime,
        std::string *context);

    Status readToByte(
        char *buffer, 
        uint64_t buffersize,
        int64_t *modifytime,
        int64_t *createtime);

    Status getFileSize(uint64_t *filesize);

    int getSystemErrno() {
        return errno_;
    }

    const char *buffer();
 private:
    int fd_;

    int errno_;

};

class WriteFile : public nocopyable {
 public:
    explicit WriteFile(const std::string &file_name);

    ~WriteFile();

    Status flush();

    Status append(const char* context, const uint64_t len);

    int getSystemError() const;

    uint64_t getWritenBytes() const;
 private:
    FILE *fp_;

    int errno_;

    int writen_bytes_;

};
}


#endif

