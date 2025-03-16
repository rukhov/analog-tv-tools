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

class comb_feedback
{
    const int32_t _N;
    std::vector<float> _filer_state;
    int32_t _filter_index;
    double _filter_sum = {};
    float _state = {};
    std::vector<float> _buffer;

public:
    comb_feedback(size_t N);
    float state() const { return _state; }
    float process(float s);
    std::span<float> process(std::span<const float> const& data);
};
} // namespace dsp