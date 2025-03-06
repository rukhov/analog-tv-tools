#pragma once

namespace dsp {

template <typename T>
class differentiator
{
    T _prev_s = 0;
    T _state = 0;
    double _tau;

public:
    differentiator(double tau) : _tau(tau) { assert(_tau > 0); }

    T state() const { return _state; }

    T process(T s)
    {
        auto diff = s - _prev_s;
        _prev_s = s;
        _state += -(_state / _tau) + diff;
        return _state;
    }
};
} // namespace dsp