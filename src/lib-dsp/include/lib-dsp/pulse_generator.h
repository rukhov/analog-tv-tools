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

#include "processor.h"

namespace dsp {

template <typename T>
class pulse_generator : public dsp::processor<T>
{
    std::vector<T> _buffer;
    const double _frequency;
    const double _corrector;
    const double _soft_corrector;
    double _phase = 0.3f;

public:
    using in_span_t = processor<float>::in_span_t;
    using out_span_t = processor<float>::out_span_t;

    pulse_generator(double frequency)
        : _frequency(frequency),
          _corrector(frequency / 8.),
          _soft_corrector(frequency / 64.)
    {
    }

    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        auto out = std::span<T>(_buffer.data(), in.size());

        for (uint32_t i = 0; i < in.size(); ++i) {
            out[i] = _process(in[i]);
        }

        return out;
    }

    T _process(T s)
    {
        if (s > 0) {
            /*
                if (_phase < 0.2)
                    _phase -= _soft_corrector;
                else if (_phase < 0.5)
                    _phase -= _corrector;
                else if (_phase > 0.8)
                    _phase += _soft_corrector;
                else
                    _phase += _corrector;
                    */
            if (_phase < 0.5)
                _phase -= _corrector;
            else
                _phase += _corrector;
        }

        _phase += _frequency;

        if (_phase > 1) {
            _phase -= 1;
            s = 1;
        } else {
            s = 0;
        }

        return s;
    }
};
} // namespace dsp