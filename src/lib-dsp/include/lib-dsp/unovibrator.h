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
class unovibrator
{
    const uint64_t _delay;
    const uint64_t _delay_plus_pulse_length;
    T _state = -1;

    uint64_t _cycles_since_state_signaled = {};

public:
    unovibrator(uint64_t delay, uint64_t pulse_length)
        : _delay(delay), _delay_plus_pulse_length(pulse_length + delay)
    {
    }

    T state() const { return _state; }

    T process(T s)
    {
        if (_cycles_since_state_signaled > 0 || (s > 0 && _state <= 0)) {

            if (_cycles_since_state_signaled >= _delay_plus_pulse_length) {
                _state = -1;
                _cycles_since_state_signaled = 0;
            } else if (_cycles_since_state_signaled >= _delay) {
                ++_cycles_since_state_signaled;
                _state = 1;
            } else {
                _state = -1;
                ++_cycles_since_state_signaled;
            }
        }
        return state();
    }
};
} // namespace dsp