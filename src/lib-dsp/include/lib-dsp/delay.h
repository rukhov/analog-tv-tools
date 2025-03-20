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

#include <boost/circular_buffer.hpp>

#include "processor.h"

namespace dsp {

template <typename T>
class delay : public processor<T>
{
    boost::circular_buffer<T> _buffer;
    std::vector<T> _out_buffer;

    T _process(T s)
    {
        _buffer.push_back(s);
        return state();
    }

public:
    using in_span_t = processor<T>::in_span_t;
    using out_span_t = processor<T>::out_span_t;

    delay(size_t len) : _buffer(len, len, T{}) {}

    size_t capacity() const { return _buffer.capacity(); }
    size_t length() const { return _buffer.size(); }

    T state() const { return _buffer.front(); }

    T process(T s) override { return _process(s); }

    out_span_t process(in_span_t const& in) override
    {
        if (_out_buffer.size() < in.size())
            _out_buffer.resize(in.size());

        auto out = std::span<T>(_out_buffer.data(), in.size());

        for (uint32_t i = 0; i < in.size(); ++i) {
            out[i] = _process(in[i]);
        }

        return out;
    }
};
} // namespace dsp