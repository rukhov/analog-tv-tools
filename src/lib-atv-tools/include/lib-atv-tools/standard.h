#pragma once

namespace atv {

enum class standard_e { NTSC = 0, PAL };

struct standard {
    standard_e standard;
    size_t total_line_count;
    bool interlaced;
    double chroma_band_center_hz;
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
};
} // namespace atv