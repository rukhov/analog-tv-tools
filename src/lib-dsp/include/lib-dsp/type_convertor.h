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

template <typename Tin, typename Tout, Tin tap>
class type_convertor : public dsp::processor<Tin, Tout>
{
    std::vector<Tout> _buffer;

public:
    using in_span_t = processor<Tin, Tout>::in_span_t;
    using out_span_t = processor<Tin, Tout>::out_span_t;

    out_span_t process(in_span_t const& in)
    {
        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        auto src = in.begin();
        auto src_end = in.end();
        auto dst = _buffer.begin();

        for (; src != src_end; ++src, ++dst) {
            *dst = static_cast<Tout>((*src) * tap);
        }

        return { _buffer.data(), in.size() };
    }
};
} // namespace dsp