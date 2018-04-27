#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>
#include <algorithm>
#include <vector>

#include <FileUtils.hpp>
namespace Base {

ReadFile::ReadFile(const std::string &filename) : 
    fd_(open(filename.c_str(), O_RDONLY | O_CLOEXEC)), 
    errno_(0) {
        if (fd_ < 0) {
            errno_ = errno;
        }
    }

ReadFile::~ReadFile() {
    if (fd_ > 0) {
        close(fd_);
    }
}

Status ReadFile::getFileSize(uint64_t *filesize) {
    struct stat statbuf;
    if (fd_ > 0) {
        if (0 == fstat(fd_, &statbuf)) {
            *filesize = statbuf.st_size;
            return SUCCESS;
        } else {
            *filesize = 0;
            errno_ = errno;
            return FILE_GET_STATUS_FAILED;
        }
    } else {
        *filesize = 0;
        errno_ = errno;
        return FILE_OPEN_FAILED;
    }
}

Status ReadFile::readToString(
        uint64_t readsize,
        int64_t *modifytime, //since 1970 to now seconds
        int64_t *createtime,
        std::string *context) {
    Status status = SUCCESS;
    if (fd_ >= 0) {
        context->clear();
        struct stat statbuf;
        if (0 == fstat(fd_, &statbuf)) {
            if (S_ISREG(statbuf.st_mode)) {
                uint64_t filesize = 0;
                status = getFileSize(&filesize);
                if (SUCCESS != status) {
                    std::cout << "get file size filed." << std::endl;
                    return status;
                }
                context->reserve(static_cast<int>(std::min(readsize, filesize)));
            } else if (S_ISDIR(statbuf.st_mode)) {
                errno_ = EISDIR;
                return FILE_FORMAT_INVALID;
            }
            if (modifytime) {
                *modifytime = statbuf.st_mtime;
            } 
            if (createtime) {
                *createtime = statbuf.st_ctime;
            }
        } else {
            errno_ = errno;
            return FILE_GET_STATUS_FAILED;
        }
    } else {
        errno_ = errno;
        return FILE_OPEN_FAILED;
    }
    std::vector<char> buffer_vec(readsize);

    char *buffer = buffer_vec.data();
    if (NULL == buffer) {
        std::cout << "not have enough memory" << std::endl;
        return MALLOC_MEMORY_FAILED;
    }
    while (context->size() < static_cast<size_t>(readsize)) {
        size_t toRead = static_cast<size_t>(readsize) - context->size();
        ssize_t readnum = read(fd_, buffer, toRead);
        if (readnum > 0) {
            context->append(buffer, readnum);
        } else {
            if (readnum < 0) {
                errno_ = errno_;
                return FILE_READ_FAILED;
            }
            break;
        }
    }
    return SUCCESS;
}

Status ReadFile::readToByte(
    char *buffer, 
    uint64_t buffersize,
    int64_t *modifytime,
    int64_t *createtime) {
    if (NULL == buffer) {
        std::cout << "input ptr is null" << std::endl;
        return MALLOC_MEMORY_FAILED;
    }
    if (fd_ >= 0) {
        struct stat statbuf;
        if (0 == fstat(fd_, &statbuf)) {
            if (S_ISDIR(statbuf.st_mode)) {
                errno_ = EISDIR;
                return FILE_FORMAT_INVALID;
            }
            if (modifytime) {
                *modifytime = statbuf.st_mtime;
            } 
            if (createtime) {
                *createtime = statbuf.st_ctime;
            }
        } else {
            errno_ = errno;
            return FILE_GET_STATUS_FAILED;
        }
    } else {
        errno_ = errno;
        return FILE_OPEN_FAILED;
    }
    size_t count = 0;
    size_t readnum = 0;
    while (count < static_cast<size_t>(buffersize)) {
        size_t toRead = static_cast<size_t>(buffersize) - count;
        readnum = read(fd_, buffer, toRead);
        if (readnum > 0) {
            buffer = buffer + readnum;
            count = count + readnum;
        } else {
            if (readnum < 0) {
                errno_ = errno_;
                return FILE_READ_FAILED;
            }
            break;
        }
    }

    return SUCCESS;
}

WriteFile::WriteFile(const std::string &filename) :
    fp_(fopen(filename.c_str(), "ae")),
    errno_(0),
    writen_bytes_(0) {}

WriteFile::~WriteFile() {
    if (NULL != fp_) {
        fclose(fp_);
    }
}

Status WriteFile::flush() {
    if (NULL != fp_) {
        int ret = ::fflush(fp_);
        if (0 != ret) {
            return FILE_FFLUSH_FAILED;
        }
        return SUCCESS;
    } else {
        return FILE_WRITE_FAILED;    
    }
}

Status WriteFile::append(const char *context, const uint64_t len) {
    if (NULL != fp_) {
        size_t count = ::fwrite(context, 1, len, fp_);
        if (len != count) {
            int err = ferror(fp_);
            if (err) {
                errno_ = err;
                return FILE_APPEND_FAILED;
            }
        }
        writen_bytes_ += len;
        return SUCCESS;
    } else {
        return FILE_OPEN_FAILED;
    }
}

int WriteFile::getSystemError() const {
    return errno_;
}

uint64_t WriteFile::getWritenBytes() const {
    return writen_bytes_;
}
}
