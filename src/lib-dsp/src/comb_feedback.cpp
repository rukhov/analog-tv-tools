#include "../include/lib-dsp/comb_feedback.h"
#include <siglib.h>


namespace {

} // namespace

namespace dsp {

comb_feedback::comb_feedback(size_t N)
    : _N(N), _filer_state(std::vector<double>::size_type{ N })
{
    assert(N > 0);
    ::SIF_FixedDelay(_filer_state.data(), // Pointer to delay state array
                     &_filter_index,      // Pointer to delay state index
                     N                    // Delay length
    );
}

float comb_feedback::process(float s)
{
    _state = s - SDS_FixedDelay(s,                   // Input sample to be filtered
                                _filer_state.data(), // Pointer to delay state array
                                &_filter_index,      // Pointer to delay state index
                                _N);                 // Delay length

    return state();
}
} // namespace dsp
