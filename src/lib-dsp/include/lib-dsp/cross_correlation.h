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
#include <boost/circular_buffer.hpp>

namespace dsp {

template <typename T>
class cross_correlation : public processor<T>
{
    std::vector<T> _buffer;
    boost::circular_buffer<T> _state;
    T _sum = 0;
    const size_t _pulse_length;

public:
    using in_span_t = processor<T>::in_span_t;
    using out_span_t = processor<T>::out_span_t;

    cross_correlation(size_t pulse_length) : _pulse_length(pulse_length)
    {
        _state.resize(pulse_length, 0);
    }

    size_t get_pulse_length() const { return _pulse_length; }

    T _process(T s)
    {
        _state.push_back(s);

        _sum += _state.back();
        _sum -= _state.front();

        return _sum;
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
};
} // namespace dsp