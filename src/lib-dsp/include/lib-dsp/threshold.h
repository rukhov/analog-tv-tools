#pragma once

namespace dsp {

template <typename T>
class threshold
{
    T _on_level;
    T _off_level;
    T _state;

public:
    threshold(T level, T histerezis)
        : _on_level(level + histerezis), _off_level(level - histerezis), _state(0)
    {
        assert(histerezis >= 0);
    }

    T state() const { return _state; }

    T process(T s)
    {
        if (s > _on_level && _state <= 0) {
            _state = 1;
        } else if (s < _off_level && _state >= 0) {
            _state = -1;
        }
        return _state;
    }
};
} // namespace dsp