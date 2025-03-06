#pragma once

namespace dsp {

template <typename T>
class pulse_detector
{
    uint64_t _min_pulse_length;
    uint64_t _samples_from_state_change = 0;
    bool _inversed_pulse;
    T _on_pulse_level;
    T _off_pulse_level;
    T _prev_s = 0;
    bool _state = false;

public:
    pulse_detector(uint64_t pulse_minimum_length_cicles, T pulse_level, T histerezis)
        : _min_pulse_length(pulse_minimum_length_cicles),
          _on_pulse_level(pulse_level + histerezis / 2),
          _off_pulse_level(pulse_level - histerezis / 2)
    {
        assert(pulse_minimum_length_cicles > 0);
        assert(histerezis > 0);
    }

    T state() const
    {
        if (!_state || _samples_from_state_change < _min_pulse_length)
            return -1;

        return 1;
    }

    T process(const T s)
    {
        ++_samples_from_state_change;

        if (_state) {
            if (s <= _off_pulse_level && _prev_s > _off_pulse_level) {
                _state = false;
                _samples_from_state_change = 0;
            }
        } else {
            if (s >= _on_pulse_level && _prev_s < _on_pulse_level) {
                _state = true;
                _samples_from_state_change = 0;
            }
        }

        _prev_s = s;

        return state();
    }
};
} // namespace dsp