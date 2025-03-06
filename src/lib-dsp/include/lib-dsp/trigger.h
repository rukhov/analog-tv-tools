#pragma once

namespace dsp {

template <typename T>
class trigger
{
    T _on_level;
    const bool _positive = true;
    T _prev_sample = 0;
    T _state = 0;

public:
    trigger(T level, bool positive = true) : _on_level(level), _positive(positive) {}

    T state() const { return _state; }

    void reset(T s) { _state = s; }

    T process(T s)
    {
        if (_positive) {
            if (s >= _on_level && _prev_sample < _on_level) {
                _state = 1;
            } else {
                _state = -1;
            }
        } else {
            if (s <= _on_level && _prev_sample > _on_level) {
                _state = 1;
            } else {
                _state = -1;
            }
        }
        _prev_sample = s;
        return _state;
    }
};
} // namespace dsp