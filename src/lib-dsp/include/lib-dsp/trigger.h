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
class trigger : public processor<T>
{
    T _on_level;
    const bool _positive = true;
    T _prev_sample = 0;
    T _state = 0;
    std::vector<T> _buffer;

public:
    using in_span_t = processor<T>::in_span_t;
    using out_span_t = processor<T>::out_span_t;

    trigger(T level, bool positive = true) : _on_level(level), _positive(positive) {}

    T state() const { return _state; }

    void reset(T s) { _state = s; }

    inline T _process(T s)
    {
        if (_positive) {
            if (s >= _on_level && _prev_sample < _on_level) {
                _state = 1;
            } else {
                _state = -1;
            }
        } else {
            if (s <= _on_level && _prev_sample > _on_level) {
                _state = 1;
            } else {
                _state = -1;
            }
        }
        _prev_sample = s;
        return _state;
    }

    T process(T s) override { return _process(s); }

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