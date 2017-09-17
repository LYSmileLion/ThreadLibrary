#include <iostream>
#include <string>
#include <LogFile.hpp>
using namespace HPCs;
int main(){
	char num[] = {'\0'};
	std::string str("sdd");
	std::cout << num << std::endl;
	std::cout << "str len" << str.size() << std::endl;
	LogFile logfile("./sdd",7800);
	logfile.append(num,1);
	

}
