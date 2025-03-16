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

    std::span<T> process_inplace(std::span<T> const& s)
    {
        for (auto& v : s) {
            v = process(v);
        }
        return s;
    }
};
} // namespace dsp