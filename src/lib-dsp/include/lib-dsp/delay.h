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

namespace dsp {

template <typename T>
class delay
{
    boost::circular_buffer<T> _buffer;

public:
    delay(size_t len) : _buffer(len, len, T{}) {}

    size_t capacity() const { return _buffer.capacity(); }
    size_t length() const { return _buffer.size(); }

    T state() const { return _buffer.front(); }

    T process(T s)
    {
        _buffer.push_back(s);
        return state();
    }
};
} // namespace dsp