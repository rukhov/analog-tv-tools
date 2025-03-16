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

#include "standard.h"

namespace atv {


standard standard::make(standard_e standard)
{

    if (standard == standard_e::SECAM) {
        // SECAM
        constexpr auto Nline = 625;
        constexpr auto Fh = 15625.;
        constexpr auto Fv = 2 * Fh / Nline;
        constexpr auto V_us = 1000000 / Fv;
        constexpr auto H_us = 1000000 / Fh;
        return { .standard = atv::standard_e::SECAM,
                 .total_line_count = Nline,
                 .interlaced = true,
                 .chroma_subcarrier1_hz = 4250000.,
                 .chroma_subcarrier2_hz = 4406250.,
                 //.chroma_band_width_hz = 1168250.,
                 .chroma_band_width_hz = 1850000.,
                 .H_us = H_us,
                 .timing_tolerance_us = 1.2,
                 .chroma_burst_duration_us = 2.25,
                 .hsync_pulse_length_us = 4.7,
                 .vsync_pulse_frequency_hz = Fv,
                 .hsync_pulse_frequency_hz = Fh,
                 .h_ref_point_to_burst_styart_us = 5.6,
                 .pre_equalisation_pulse_length_us = 2.35,
                 .vertical_serration_pulse_length_us = 27.3 };
    } else if (standard == standard_e::NTSC) {
        constexpr auto Nline = 525;
        constexpr auto Fsc = 63. * 5000000. / 88.;
        constexpr auto Fh = 2 * Fsc / 455.;
        constexpr auto Fv = 2 * Fh / Nline;
        constexpr auto V_us = 1000000 / Fv;
        constexpr auto H_us = 1000000 / Fh;
        constexpr auto Hsc_us = 1000000 / Fsc;
        return { .standard = atv::standard_e::NTSC,
                 .total_line_count = Nline,
                 .interlaced = true,
                 .chroma_subcarrier1_hz = Fsc,
                 .chroma_subcarrier2_hz = Fsc,
                 .chroma_band_width_hz = 1800000.,
                 .H_us = H_us,
                 .timing_tolerance_us = 1.2,
                 .chroma_burst_duration_us = 2.5,
                 .hsync_pulse_length_us = 4.7,
                 .vsync_pulse_frequency_hz = Fv,
                 .hsync_pulse_frequency_hz = Fh,
                 .h_ref_point_to_burst_styart_us = 19 * Hsc_us,
                 .pre_equalisation_pulse_length_us = H_us * .04,
                 .vertical_serration_pulse_length_us = (H_us / 2 - H_us * .07) };
    } else if (standard == standard_e::PAL) {
        // PAL 625
        constexpr auto Nline = 625;
        constexpr auto Fh = 15625.;
        constexpr auto Fsc = (1135. / 4. + 1. / Nline) * Fh; // 4433618.75;
        constexpr auto Fv = 2 * Fh / Nline;
        constexpr auto V_us = 1000000 / Fv;
        constexpr auto H_us = 1000000 / Fh;
        constexpr auto Hsc_us = 1000000 / Fsc;
        return { .standard = atv::standard_e::PAL,
                 .total_line_count = Nline,
                 .interlaced = true,
                 .chroma_subcarrier1_hz = Fsc,
                 .chroma_subcarrier2_hz = Fsc,
                 .chroma_band_width_hz = 1800000.,
                 .H_us = H_us,
                 .timing_tolerance_us = 1.2,
                 .chroma_burst_duration_us = 2.25,
                 .hsync_pulse_length_us = 4.7,
                 .vsync_pulse_frequency_hz = Fv,
                 .hsync_pulse_frequency_hz = Fh,
                 .h_ref_point_to_burst_styart_us = 5.6,
                 .pre_equalisation_pulse_length_us = 2.35,
                 .vertical_serration_pulse_length_us = 27.3 };
    }

    std::unreachable();
}
} // namespace atv
