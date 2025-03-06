#pragma once

namespace dsp {

class comb_feedforward
{
    const int32_t _N;
    std::vector<float> _filer_state;
    int32_t _filter_index;
    float _filter_sum = {};
    const float _1_div_N;
    float _state = {};
    std::vector<float> _buffer;

public:
    comb_feedforward(size_t N);
    float state() const { return _state; }
    float process(float s);
    std::span<float> process(std::span<float const> const& s);
};
} // namespace dsp