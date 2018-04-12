#include <algorithm>

#include <utils.hpp>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

const char digitsHex[] = "0123456789ABCDEF";

template<typename T>
int convert(char buf[], T value) {
    T i = value;
    char* p = buf;
    do {
      int lsd = static_cast<int>(i % 10);
      i /= 10;
      *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
      *p++ = '-';
    }
    *p = '\0';

    std::reverse(buf, p);

    return p - buf;
}

template int convert(char buf[], int value);
template int convert(char buf[], unsigned int value);
template int convert(char buf[], long long value);
template int convert(char buf[], unsigned long long value);
template int convert(char buf[], long value);
template int convert(char buf[], unsigned long value);

int convertHex(char buf[], uintptr_t value) {//uintptr_t 表示一个可以装的下指针的整形
  uintptr_t i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 16);
    i /= 16;
    *p++ = digitsHex[lsd];
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

/*template <typename unsigned int>
int convert(char buf[], unsigned int value);
template <typename short>
int convert(char buf[], short value);
template <typename unsigned short>
int convert(char buf[], unsigned short value);
*/
