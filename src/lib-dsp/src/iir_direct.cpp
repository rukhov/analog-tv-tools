/*
 * This file is part of the analot-tv-tools project.
 * Copyright (c) 2025 Roman Ukhov.
 *
 * https://github.com/rukhov/analog-tv-tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lib-dsp/iir.h"

#include <boost/circular_buffer.hpp>

namespace {
template <typename T>
class iir_direct_1_impl : public dsp::processor<T, T>
{
    std::vector<T> _b;
    std::vector<T> _a;
    boost::circular_buffer<T> _x;
    boost::circular_buffer<T> _y;
    std::vector<T> _buffer;

public:
    using out_span_t = dsp::processor<T, T>::out_span_t;
    using in_span_t = dsp::processor<T, T>::in_span_t;

    iir_direct_1_impl(std::span<const T> const& taps)
        : _b(taps.begin(), taps.begin() + (taps.size() / 2 + 1)),
          _a(taps.begin() + (taps.size() / 2 + 1), taps.end()),
          _x(taps.size() / 2 + 1, T{}),
          _y(taps.size() / 2, T{})
    {
        assert((_b.size() % 2) == 0);
        assert((_a.size() % 2) == 1);
        assert((_x.size() % 2) == 0);
        assert((_y.size() % 2) == 1);

        //_f.setupN(centerFrequencyNormalised, bandWidthNormalised);
    }

    inline T _process(T s)
    {
        T y = {};

        _x.push_back(s);

        for (size_t i = 0; i < _b.size() - 1; ++i) {
            y += _b[i] * _x[i];
        }
        _x.push_back(s);

        for (size_t i = 0; i < _a.size(); ++i) {
            y -= _a[i] * _y[i];
        }
        _y.push_back(y);

        return y;
    }

    // processor
private:
    T process(T s) override { return {}; };

    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        for (size_t i = 0; i < in.size(); ++i) {
            _buffer[i] = _process(in[i]);
        }

        return { _buffer.data(), in.size() };
    }
};
} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, float>>
make_direct<float>(std::span<const float> const& taps)
{
    return std::make_unique<iir_direct_1_impl<float>>(taps);
}

} // namespace dsp