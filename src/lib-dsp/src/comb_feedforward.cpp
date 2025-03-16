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

#include "../include/lib-dsp/comb_feedforward.h"
#include <siglib.h>


namespace {

} // namespace

namespace dsp {

comb_feedforward::comb_feedforward(size_t N)
    : _N(N), _1_div_N(1. / N), _filer_state(std::vector<float>::size_type{ N })
{
    assert(N > 0);
    ::SIF_Comb(_filer_state.data(), // SLData_t *,Pointer to filter state array
               &_filter_index,      // SLArrayIndex_t *,Pointer to filter index register
               &_filter_sum,        //*, Pointer to filter sum register
               N                    // const SLArrayIndex_t Filter length
    );
}

float comb_feedforward::process(float s)
{
    _state = SDS_Comb(s,                   // Input sample to be filtered
                      _filer_state.data(), // SLData_t *,Pointer to filter state array
                      &_filter_index,      // SLArrayIndex_t *,Filter index pointer
                      &_filter_sum,        // SLData_t *,Filter sum register pointer
                      _N);

    _state *= _1_div_N;

    return state();
}

std::span<float> comb_feedforward::process(std::span<float const> const& s)
{
    if (_buffer.size() < s.size())
        _buffer.resize(s.size());

    SDA_Comb(s.data(),
             _buffer.data(),
             _filer_state.data(),
             &_filter_index,
             &_filter_sum,
             _N,
             s.size());

    SDA_Multiply(_buffer.data(), _1_div_N, _buffer.data(), s.size());

    return { _buffer.data(), s.size() };
}

} // namespace dsp
