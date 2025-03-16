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
#include "lib-dsp/quadrature_demod.h"
#include "lib-dsp/utils.h"


#include <siglib.h>

namespace {

using namespace dsp;

constexpr size_t SINE_TABLE_SIZE = 1024 * 10; // Look up table for fast sine calculation

std::array<float, SINE_TABLE_SIZE> g_sine_table;

const float* getSineTable()
{
    auto fillTable = []() -> const float* {
        SIF_FastSin(g_sine_table.data(), g_sine_table.size());
        return g_sine_table.data();
    };

    static const float* table = fillTable();
    return table;
}

template <typename T>
class quadrature_demod_impl : public dsp::processor<T, std::complex<T>>
{
    const float* const _sine_table;
    double _phase = 0;
    double _carrierFrequency;
    std::vector<std::complex<T>> _buffer;
    std::vector<std::complex<T>> _out_buffer;
    std::unique_ptr<dsp::processor<std::complex<float>>> _quad_high_pass;
    std::complex<float> _prev_sample = {};

    void add_pahse(double adder)
    {
        _phase += adder;
        _phase = dsp::normalize_phase(_phase);
    }

    T getSine(double p) const
    {
        return std::sin(p * dsp::DSP_2PI);

        size_t idx = (size_t)((p * ((double)SINE_TABLE_SIZE)));

        if (idx >= SINE_TABLE_SIZE) {
            return 0;
        }

        SLData_t Dst = _sine_table[idx];

        return Dst;
    }

public:
    using out_span_t = dsp::processor<T, std::complex<T>>::out_span_t;
    using in_span_t = dsp::processor<T, std::complex<T>>::in_span_t;

    quadrature_demod_impl(double carrierFrequency)
        : _carrierFrequency(carrierFrequency),
          _sine_table(getSineTable()),
          _quad_high_pass(
              dsp::make_high_pass<std::complex<float>, 5>(carrierFrequency + .1))
    {
    }

    // processor
private:
    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size()) {
            _buffer.resize(in.size());
            _out_buffer.resize(in.size());
        }

        for (size_t i = 0; i < in.size(); ++i) {

            auto s = getSine(_phase);
            auto c = getSine(dsp::normalize_phase(_phase + .25));
            add_pahse(_carrierFrequency);

            _buffer[i].real(c * in[i]);
            _buffer[i].imag(s * in[i]);
        }

        // return { _buffer.data(), in.size() };

        auto filtered = _quad_high_pass->process({ _buffer.data(), in.size() });
        // auto filtered = std::span<std::complex<float>>(_buffer.data(), in.size());

        // return filtered;

        for (size_t i = 0; i < filtered.size(); ++i) {

            _out_buffer[i] = filtered[i];
            auto prev = filtered[i];
            _prev_sample.imag(-_prev_sample.imag());
            filtered[i] = filtered[i] * _prev_sample;
            _prev_sample = prev;

            _out_buffer[i].real(
                1. *
                (0. + atan2f(filtered[i].imag(), filtered[i].real()) / dsp::DSP_2PI));
        }

        return { _out_buffer.data(), in.size() };
    }
};
} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, std::complex<float>>>
make_quadrature_demod(double currierFrequency)
{
    return std::make_unique<quadrature_demod_impl<float>>(currierFrequency);
}

} // namespace dsp