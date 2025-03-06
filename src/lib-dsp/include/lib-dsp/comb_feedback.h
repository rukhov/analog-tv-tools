#pragma once

namespace dsp {

class comb_feedback
{
    const int32_t _N;
    std::vector<float> _filer_state;
    int32_t _filter_index;
    double _filter_sum = {};
    float _state = {};

public:
    comb_feedback(size_t N);
    float state() const { return _state; }
    float process(float s);
};
} // namespace dsp