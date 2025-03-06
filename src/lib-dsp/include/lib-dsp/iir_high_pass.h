#pragma once

namespace dsp {

class iir_high_pass
{
    int32_t stage_number_;
    std::vector<float> IIRCoeffs_;
    std::vector<float> FilterState_;
    std::vector<float> _buffer;
    float _state = 0;

public:
    iir_high_pass(double cutoff_frequency);
    float state() const { return _state; }
    float process(float s);
    std::span<float> process(std::span<float const> const& s);
};
} // namespace dsp