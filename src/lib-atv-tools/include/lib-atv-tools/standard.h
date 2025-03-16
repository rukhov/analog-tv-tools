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

namespace atv {

enum class standard_e { SECAM = 0, NTSC = 1, PAL = 2 };

struct standard {
    standard_e standard;
    size_t total_line_count;
    bool interlaced;
    double chroma_subcarrier1_hz;
    double chroma_subcarrier2_hz;
    double chroma_band_width_hz;
    double H_us;
    double timing_tolerance_us;
    double chroma_burst_duration_us;
    double hsync_pulse_length_us;
    double vsync_pulse_frequency_hz;
    double hsync_pulse_frequency_hz;
    double h_ref_point_to_burst_styart_us;
    double pre_equalisation_pulse_length_us;
    double vertical_serration_pulse_length_us;

    static atv::standard make(standard_e);
};

} // namespace atv