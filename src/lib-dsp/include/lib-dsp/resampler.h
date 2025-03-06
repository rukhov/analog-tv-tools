#pragma once

#include "processor.h"

namespace dsp {

class resampler : public dsp::processor<float>
{
    void* _resampler;
    std::vector<float> _buff;
    const double _ratio;

public:
    resampler(double isr, double osr);
    ~resampler();

    float state() const { return {}; }
    span process(span const& data);
};
} // namespace dsp