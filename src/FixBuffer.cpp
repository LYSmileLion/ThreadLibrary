#include <FixBuffer.hpp>

using namespace HPCs;

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart() {}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd() {}


template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;
