#pragma once

namespace dsp {

template <typename T>
class limiter
{
    T _min;
    T _max;

public:
    limiter(T min_, T max_) : _min(min_), _max(max_) {}

    auto process(T s) const
    {
        if (s < _min)
            s = _min;
        if (s > _max)
            s = _max;
        return s;
    }
};
} // namespace dsp