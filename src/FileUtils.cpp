#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>
#include <algorithm>

#include <FileUtils.hpp>

using namespace HPCs;

extern int errno;

ReadFile::ReadFile(const std::string &filename) : 
	fd_(open(filename.c_str(), O_RDONLY | O_CLOEXEC)), 
	errno_(0) {
	if (fd_ < 0) {
		errno_ = errno;
	}
}

ReadFile::~ReadFile() {
	if (errno_ > 0) {
		close(fd_);
	}
}

HPCsStatus ReadFile::getFileSize(uint64_t *filesize) {
	struct stat statbuf;
	if (0 == fstat(fd_, &statbuf)) {
		*filesize = statbuf.st_size;
		return HPCS_SUCCESS;
	} else {
		*filesize = 0;
		errno_ = errno;
		return HPCS_FILE_OPEN_FAILED;
	}
}

HPCsStatus ReadFile::readToString(
	uint64_t readsize,
	int64_t *modifytime,
	int64_t *createtime,
	std::string *context) {
	HPCsStatus status = HPCS_SUCCESS;
	char *buffer = new char[readsize];
	if (NULL == buffer) {
		std::cout << "not have enough memory" << std::endl;
		return HPCS_MALLOC_MEMORY_FAILED;
	}
	if (fd_ >= 0) {
		context->clear();
		struct stat statbuf;
		if (0 == fstat(fd_, &statbuf)) {
			if (S_ISREG(statbuf.st_mode)) {
				uint64_t filesize = 0;
				status = getFileSize(&filesize);
				if (HPCS_SUCCESS != status) {
					std::cout << "get file size filed." << std::endl;
					return status;
				}
				context->reserve(static_cast<int>(std::min(readsize, filesize)));
			} else if (S_ISDIR(statbuf.st_mode)) {
				errno_ = EISDIR;
				return HPCS_FILE_FORMAT_INVALID;
			}
			if (modifytime) {
				*modifytime = statbuf.st_mtime;
			} 
			if (createtime) {
				*createtime = statbuf.st_ctime;
			}
		} else {
			errno_ = errno;
			return HPCS_FILE_GET_STATUS_FAILED;
		}
	} else {
		errno_ = errno;
		return HPCS_FILE_OPEN_FAILED;
	}

	while (context->size() < static_cast<size_t>(readsize)) {
		size_t toRead = static_cast<size_t>(readsize) - context->size();
		ssize_t readnum = read(fd_, buffer, toRead);
		if (readnum > 0) {
			context->append(buffer, readnum);
		} else {
			if (readnum < 0) {
				errno_ = errno_;
				return HPCS_FILE_READ_FAILED;
			}
			break;
		}
	}
	delete[] buffer;
	return HPCS_SUCCESS;

}
HPCsStatus ReadFile::readToByte(
	char *buffer, 
	uint64_t buffersize,
	int64_t *modifytime,
	int64_t *createtime) {
	if (NULL == buffer) {
		std::cout << "input ptr is null" << std::endl;
		return HPCS_MALLOC_MEMORY_FAILED;
	}
	if (fd_ >= 0) {
		struct stat statbuf;
		if (0 == fstat(fd_, &statbuf)) {
		    if (S_ISDIR(statbuf.st_mode)) {
				errno_ = EISDIR;
				return HPCS_FILE_FORMAT_INVALID;
			}
			if (modifytime) {
				*modifytime = statbuf.st_mtime;
			} 
			if (createtime) {
				*createtime = statbuf.st_ctime;
			}
		} else {
			errno_ = errno;
			return HPCS_FILE_GET_STATUS_FAILED;
		}
	} else {
		errno_ = errno;
		return HPCS_FILE_OPEN_FAILED;
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
				return HPCS_FILE_READ_FAILED;
			}
			break;
		}
	}

	return HPCS_SUCCESS;
}

WriteFile::WriteFile(const std::string &filename) :
	fp_(fopen(filename.c_str(), "ae")),
	errno_(0),
	writen_bytes_(0) {
	
}

WriteFile::~WriteFile() {
	fclose(fp_);
}

void WriteFile::flush() {
	fflush(fp_);
}

void WriteFile::append(const char *context, const uint64_t len) {
	size_t writecount =  fwrite(context, 1, len, fp_);
	size_t remain = len - writecount;
	while (remain > 0) {
		size_t writenum = fwrite(context + writecount, 1, remain, fp_);
		if (static_cast<size_t>(0) == writenum) {
			int err = ferror(fp_);
			if (err) {
				std::cout << "append failed" << std::endl;
				errno_ = err;
			}
			break;
		}
		writecount += writenum;
		remain = len - writecount;
	}
}

int WriteFile::getSystemError() const {
	return errno_;
}

uint64_t WriteFile::getWritenBytes() const {
	return writen_bytes_;
}
