#pragma once

namespace dsp {

template <typename T>
class integrator
{
    T _state = 0;
    double _tau;

public:
    integrator(double tau) : _tau(tau) { assert(tau > 0); }

    T state() const { return _state; }

    T process(T s)
    {
        _state += (s - _state) * _tau;
        return _state;
    }
};
} // namespace dsp