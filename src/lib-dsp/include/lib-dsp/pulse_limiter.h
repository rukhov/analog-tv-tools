#pragma once

namespace dsp {

template <typename T>
class pulse_limiter
{
    T _diff_threshold;
    T _diff_coeff;
    T _one_minus_diff_coeff;
    T _state;

public:
    pulse_limiter(T diff_threshold_, T diff_coeff_)
        : _diff_threshold(diff_threshold_),
          _diff_coeff(diff_coeff_),
          _one_minus_diff_coeff(1 - diff_coeff_),
          _state(0)
    {
    }

    T process(T s)
    {
        auto diff = s - _state;

        if (std::abs(diff) < _diff_threshold)
            _state = s;
        else
            _state = _state * _one_minus_diff_coeff + s * _diff_coeff;

        return _state;
    }
};
} // namespace dsp