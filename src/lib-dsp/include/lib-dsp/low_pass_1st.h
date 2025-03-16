/*
 * This file is part of the analot-tv-tools project.
 * Copyright (c) 2025 Roman Ukhov.
 *
 * https://github.com/rukhov/analog-tv-tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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