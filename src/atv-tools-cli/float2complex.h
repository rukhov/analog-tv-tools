#pragma once

#include <lib-dsp/processor.h>

class float2complex : public dsp::processor<float, std::complex<float>>
{
public:
    out_span_t process(in_span_t const& in) override
    {
        return { reinterpret_cast<std::complex<float>*>(in.data()), in.size() / 2 };
    }
};