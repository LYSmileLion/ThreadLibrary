#ifndef _INCLUDE_FILEUTILS_HPP_
#define _INCLUDE_FILEUTILS_HPP_

#include <stdio.h>

#include <string>

#include <nocopyable.hpp>
#include <HPCsTypes.hpp>

namespace HPCs {

class ReadFile : public nocopyable {
 public:
	explicit ReadFile(const std::string &filename);

	~ReadFile();

	HPCsStatus readToString(
		uint64_t readsize,
		int64_t *modifytime,
		int64_t *createtime,
		std::string *context);

	HPCsStatus readToByte(
		char *buffer, 
		uint64_t buffersize,
		int64_t *modifytime,
		int64_t *createtime);

	HPCsStatus getFileSize(uint64_t *filesize);

	const char *buffer();
 private:
	int fd_;
	int errno_;
};

class WriteFile : public nocopyable {
 public:
	explicit WriteFile(const std::string &file_name);

	~WriteFile();

	void append(const char* context, const uint64_t len);

	int getSystemError() const;

	uint64_t getWritenBytes() const;
	 
	void flush();

 private:
	int errno_;
	int writen_bytes_;
	FILE *fp_;
};
}


#endif

