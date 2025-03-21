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

#include "standard.h"
#include "video_buffer.h"


namespace atv {

class decoder
{
public:
    virtual ~decoder() = default;

    virtual void process(uint64_t length,
                         float const* input,
                         short* video_yout = nullptr,
                         short* video_uout = nullptr,
                         short* video_vout = nullptr,
                         float* luma_out = nullptr,
                         float* dbg1 = nullptr,
                         float* dbg2 = nullptr,
                         float* dbg3 = nullptr) = 0;

    static std::unique_ptr<decoder> make(standard const& params,
                                         uint64_t samp_rate,
                                         bool black_and_white,
                                         video_buffer::frame_cb frame_cb);
};

} // namespace atv