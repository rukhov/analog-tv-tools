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

#include <lib-atv-tools/standard.h>

struct options {

    enum class in_type { AUTO, F32, I16 };
    enum class out_type { AVI, RAW };
    enum class in_data_type { cvbs, fm, am };

    in_type input_type = in_type::AUTO;
    in_data_type input_data_type = in_data_type::cvbs;
    out_type output_type = out_type::AVI;

    std::filesystem::path input_file;
    std::filesystem::path output_file;
    std::filesystem::path out_cvbs_raw_file;

    atv::standard_e standard = atv::standard_e::SECAM;
    double carrier_frequency_hz = 0.;
    double input_sample_rate_hz = 16000000.;
    double processing_sample_rate_hz = 16000000.;

    double dc_correction = 0.;
    double amplification = 1.;

    uint64_t max_frame_number = 0;
    bool balck_and_white = false;
};

options read_options(int atgc, char* argv[]);