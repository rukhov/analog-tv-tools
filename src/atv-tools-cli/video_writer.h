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

#include <lib-atv-tools/color.h>
#include <lib-atv-tools/standard.h>
#include <lib-atv-tools/video_buffer.h>

class video_writer
{
public:
    virtual ~video_writer() = default;
    virtual void process_frame(std::span<atv::YUV> const&,
                               size_t rect_width,
                               size_t rect_height,
                               size_t total_width,
                               size_t total_height) = 0;

    static std::unique_ptr<video_writer> make(std::filesystem::path const& file_path,
                                              atv::standard const& standard);
};
