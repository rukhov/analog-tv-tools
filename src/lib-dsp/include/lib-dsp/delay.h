#pragma once

#include <boost/circular_buffer.hpp>

namespace dsp {

template <typename T>
class delay
{
    boost::circular_buffer<T> _buffer;

public:
    delay(size_t len) : _buffer(len, len, T{}) {}

    size_t capacity() const { return _buffer.capacity(); }
    size_t length() const { return _buffer.size(); }

    T state() const { return _buffer.front(); }

    T process(T s)
    {
        _buffer.push_back(s);
        return state();
    }
};
} // namespace dsp