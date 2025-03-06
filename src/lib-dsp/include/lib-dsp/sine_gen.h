#pragma once

#include "utils.h"

namespace dsp {

class sine_gen
{
public:
    sine_gen(double carrier_frequency, double initial_phase)
        : _carrier_frequency(carrier_frequency), _phase(initial_phase)
    {
        gen_table();
    }

    double phase() const { return _phase; }

    double set_phase(double p)
    {
        _phase = normalize_phase(p);
        return _phase;
    }

    float state() const { return _state; }

    float process()
    {
        _state = get_sin();
        return state();
    }

    //
private:
    static constexpr size_t _sine_table_length = 1024;
    double _sine_table[_sine_table_length];
    double _phase; // 0..1
    float _state = {};
    double const _carrier_frequency;

    inline void gen_table()
    {
        const double phase_multiplier = DSP_2PI / _sine_table_length;
        auto ptr = _sine_table;
        for (auto i = 0; i < _sine_table_length; ++i, ++ptr) {
            *ptr = std::sin(phase_multiplier * i);
        }
    }

    double get_sin()
    {
        auto s = _sine_table[(size_t)((_phase * _sine_table_length) + 0.1)];
        _phase += _carrier_frequency;
        if (_phase >= 1.) {
            _phase -= 1.;
        }
        return s;
    }
};

} // namespace dsp