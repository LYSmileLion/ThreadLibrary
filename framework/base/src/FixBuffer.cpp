#include <FixBuffer.hpp>

using namespace Base;

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart() {}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd() {}


template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;
