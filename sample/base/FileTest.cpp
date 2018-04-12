#include <FileUtils.hpp>
#include <iostream>

int main() {
	HPCs::ReadFile read("./test");
	uint64_t filesize = 0;
	HPCsStatus status;
	status = read.getFileSize(&filesize);
	if (HPCS_SUCCESS != status) {
		std::cout << "error in read get file size" << std::endl;
		return 0;
	}
	std::cout << "file size : " << filesize << std::endl;
	std::string str1;
	status = read.readToString(
		5,
		NULL,
		NULL,
		&str1);
	if (HPCS_SUCCESS != status) {
		std::cout << "error in read[string] get file context" << std::endl;
		return 0;
	}

	std::cout << "file context : " << str1 << std::endl;
	
	char *buffer = new char[7];
	status = read.readToByte(
		buffer,
		7,
		NULL,
		NULL);
	if (HPCS_SUCCESS != status) {
		std::cout << "error in read[byte] get file context" << std::endl;
		return 0;
	}

	std::cout << "byte : ";
	for (int i = 0; i < filesize; i++) {
		std::cout << buffer[i] ;
	}
	std::cout << std::endl;

	HPCs::WriteFile write("out");
	write.append(str1.c_str(), str1.size());
	return 0;

}
