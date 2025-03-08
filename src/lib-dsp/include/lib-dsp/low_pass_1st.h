#pragma once

#include "utils.h"

namespace dsp {

template <typename T>
class low_pass_1st
{
    T _alpha; // Filter coefficient
    T _state = 0;

    static inline T calc_alpha(double fs, double fc)
    {
        auto dt = 1. / fs;
        auto tau = 1.0 / (2 * DSP_PI * fc); // Time constant (τ = RC)
        auto alpha = dt / (tau + dt);       // Filter coefficient

        return alpha;
    }

public:
    low_pass_1st(double fs, double fc) : _alpha(calc_alpha(fs, fc)) {}

    T state() const { return _state; }

    T process(T s)
    {
        // First-order IIR filter equation
        _state = _alpha * (s - _state) + _state;
        //_state = _alpha * s + (1 - _alpha) * _state;
        return _state;
    }

    std::span<T> process_inplace(std::span<T> const& s)
    {
        for (auto& v : s) {
            v = process(v);
        }
        return s;
    }
};
} // namespace dsp