#pragma once

#include "processor.h"

namespace dsp {

class resampler : public dsp::processor<float>
{
    void* _resampler;
    std::vector<float> _buff;
    const double _ratio;

public:
    using in_span_t = processor<float>::in_span_t;
    using out_span_t = processor<float>::out_span_t;

    resampler(double isr, double osr);
    ~resampler();

    float state() const { return {}; }
    out_span_t process(in_span_t const& data);
};
} // namespace dsp