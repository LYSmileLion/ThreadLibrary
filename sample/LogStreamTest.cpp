#include <LogStream.hpp>
#include <iostream>
using namespace HPCs;

int main() {
    LogStream logstream;
    logstream << 50990 ;
    std::cout << logstream.getBuffer().getDataPtr() << std::endl;

    return 0;
}
