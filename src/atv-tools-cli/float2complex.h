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

#include <lib-dsp/processor.h>

class float2complex : public dsp::processor<float, std::complex<float>>
{
    std::vector<std::complex<float>> _buffer;

public:
    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size() / 2)
            _buffer.resize(in.size() / 2);

        for (size_t i = 0; i < in.size() / 2; ++i) {
            _buffer[i].real(in[i * 2]);
            _buffer[i].imag(in[i * 2 + 1]);
        }

        return { reinterpret_cast<std::complex<float>*>(_buffer.data()), in.size() / 2 };
    }
};