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

    atv::standard_e standard = atv::standard_e::PAL;
    double carrier_frequency_hz = 0.;
    double input_sample_rate_hz = 28636363.64;
    double processing_sample_rate_hz = 28636363.64;

    double dc_correction = 0.;
    double amplification = 1.;

    uint64_t max_frame_number = 0;
};

options read_options(int atgc, char* argv[]);