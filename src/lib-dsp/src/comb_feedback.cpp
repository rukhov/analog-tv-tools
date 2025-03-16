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

#include "../include/lib-dsp/comb_feedback.h"
#include <siglib.h>


namespace {

} // namespace

namespace dsp {

comb_feedback::comb_feedback(size_t N)
    : _N(N), _filer_state(std::vector<double>::size_type{ N })
{
    assert(N > 0);
    ::SIF_FixedDelay(_filer_state.data(), // Pointer to delay state array
                     &_filter_index,      // Pointer to delay state index
                     N                    // Delay length
    );
}

float comb_feedback::process(float s)
{
    _state = s - SDS_FixedDelay(s,                   // Input sample to be filtered
                                _filer_state.data(), // Pointer to delay state array
                                &_filter_index,      // Pointer to delay state index
                                _N);                 // Delay length

    return state();
}

std::span<float> comb_feedback::process(std::span<const float> const& data)
{
    if (_buffer.size() < data.size()) {
        _buffer.resize(data.size());
    }

    for (size_t i = 0; i < data.size(); ++i) {
        _buffer[i] = process(data[i]);
    }

    return { _buffer.data(), data.size() };
}
} // namespace dsp
