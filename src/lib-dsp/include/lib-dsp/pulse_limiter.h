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
class pulse_limiter
{
    T _diff_threshold;
    T _diff_coeff;
    T _one_minus_diff_coeff;
    T _state;

public:
    pulse_limiter(T diff_threshold_, T diff_coeff_)
        : _diff_threshold(diff_threshold_),
          _diff_coeff(diff_coeff_),
          _one_minus_diff_coeff(1 - diff_coeff_),
          _state(0)
    {
    }

    T process(T s)
    {
        auto diff = s - _state;

        if (std::abs(diff) < _diff_threshold)
            _state = s;
        else
            _state = _state * _one_minus_diff_coeff + s * _diff_coeff;

        return _state;
    }
};
} // namespace dsp