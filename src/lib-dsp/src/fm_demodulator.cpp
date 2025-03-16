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

#include "lib-dsp/fm_demodulator.h"
#include "lib-dsp/utils.h"

using namespace dsp;

namespace {

template <typename T>
class fm_demodulator : public processor<T>
{
    T _prev_input = 0;
    std::vector<T> _buffer;

public:
    using in_span_t = processor<T>::in_span_t;
    using out_span_t = processor<T>::out_span_t;

    fm_demodulator() {}

    // processor<T>
private:
    T process(T s) override { return s; }
    out_span_t process(in_span_t const& data) override
    {
        if (_buffer.size() < data.size())
            _buffer.resize(data.size());

        for (size_t i = 0; i < data.size(); ++i) {
            auto prev = data[i];
            _buffer[i] = std::abs(data[i] - _prev_input);
            _buffer[i] *= DSP_2PI;
            _prev_input = prev;
        }
        return { _buffer.data(), data.size() };
    }
};
} // namespace

namespace dsp {


template <>
std::unique_ptr<processor<float>> make_fm_demodulator()
{
    return std::make_unique<fm_demodulator<float>>();
}

} // namespace dsp