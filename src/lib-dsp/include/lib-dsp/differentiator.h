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