#pragma once

#include "integrator.h"

namespace dsp {

template <typename T>
class contraphase_detector
{
    bool _state = false;
    dsp::integrator<float> _integrator;

public:
    contraphase_detector(double tau_samples) : _integrator(1 / tau_samples) {}

    bool state() const { return _state; }

    T process(T s1, T s2)
    {
        // inversion_phase_accum_ = (inversion_phase_accum_ * one_minus_ip_accum_coeff_) +
        // sample * ret_val * ip_accum_coeff_;

        if (s1 == 0 || s2 == 0)
            return _state;

        auto s = _integrator.process(s1 * s2);

        _state = s < 0 ? true : false;

        return _state;
    }
};
} // namespace dsp