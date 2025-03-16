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

class sine_gen
{
public:
    sine_gen(double carrier_frequency, double initial_phase)
        : _carrier_frequency(carrier_frequency), _phase(initial_phase)
    {
        gen_table();
    }

    double phase() const { return _phase; }

    double set_phase(double p)
    {
        _phase = normalize_phase(p);
        return _phase;
    }

    float state() const { return _state; }

    float process()
    {
        _state = get_sin();
        return state();
    }

    //
private:
    static constexpr size_t _sine_table_length = 1024;
    double _sine_table[_sine_table_length];
    double _phase; // 0..1
    float _state = {};
    double const _carrier_frequency;

    inline void gen_table()
    {
        const double phase_multiplier = DSP_2PI / _sine_table_length;
        auto ptr = _sine_table;
        for (auto i = 0; i < _sine_table_length; ++i, ++ptr) {
            *ptr = std::sin(phase_multiplier * i);
        }
    }

    double get_sin()
    {
        auto s = _sine_table[(size_t)((_phase * _sine_table_length) + 0.1)];
        _phase += _carrier_frequency;
        if (_phase >= 1.) {
            _phase -= 1.;
        }
        return s;
    }
};

} // namespace dsp