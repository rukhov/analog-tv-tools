#pragma once

namespace dsp {

class iir_band_pass
{
    const int32_t _stage_number;
    std::vector<float> _IIRCoeffs;
    std::vector<float> _FilterState;
    std::vector<float> _buffer;
    float _state = 0;

public:
    iir_band_pass(double cutoff_frequency1, double cutoff_frequency2);
    float state() const { return _state; }
    float process(float s);
    std::span<float> process(std::span<float const> const& s);
};
} // namespace dsp