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

class resampler : public dsp::processor<float>
{
    void* _resampler;
    std::vector<float> _buff;
    const double _ratio;

public:
    using in_span_t = processor<float>::in_span_t;
    using out_span_t = processor<float>::out_span_t;

    resampler(double isr, double osr);
    ~resampler();

    float state() const { return {}; }
    out_span_t process(in_span_t const& data);
};
} // namespace dsp