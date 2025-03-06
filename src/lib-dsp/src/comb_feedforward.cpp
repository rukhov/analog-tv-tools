#include "../include/lib-dsp/comb_feedforward.h"
#include <siglib.h>


namespace {

} // namespace

namespace dsp {

comb_feedforward::comb_feedforward(size_t N)
    : _N(N), _1_div_N(1. / N), _filer_state(std::vector<float>::size_type{ N })
{
    assert(N > 0);
    ::SIF_Comb(_filer_state.data(), // SLData_t *,Pointer to filter state array
               &_filter_index,      // SLArrayIndex_t *,Pointer to filter index register
               &_filter_sum,        //*, Pointer to filter sum register
               N                    // const SLArrayIndex_t Filter length
    );
}

float comb_feedforward::process(float s)
{
    _state = SDS_Comb(s,                   // Input sample to be filtered
                      _filer_state.data(), // SLData_t *,Pointer to filter state array
                      &_filter_index,      // SLArrayIndex_t *,Filter index pointer
                      &_filter_sum,        // SLData_t *,Filter sum register pointer
                      _N);

    _state *= _1_div_N;

    return state();
}

std::span<float> comb_feedforward::process(std::span<float const> const& s)
{
    if (_buffer.size() < s.size())
        _buffer.resize(s.size());

    SDA_Comb(s.data(),
             _buffer.data(),
             _filer_state.data(),
             &_filter_index,
             &_filter_sum,
             _N,
             s.size());

    SDA_Multiply(_buffer.data(), _1_div_N, _buffer.data(), s.size());

    return { _buffer.data(), s.size() };
}

} // namespace dsp
