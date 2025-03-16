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


#include <lib-dsp/fm_demodulator.h>
#include <lib-dsp/utils.h>

#include "demodulator.h"
#include "float2complex.h"

using namespace dsp;

namespace {

class demodulator : public processor<float>
{
    float2complex _float2complex;
    std::complex<float> _prev_sample = {};
    double _add = 0.;
    double _amplify = 1.;

public:
    demodulator(options::in_data_type inputType,
                double sample_rate,
                double baseband_carrier_hz)
    {
        constexpr double deviation_hz = 8000000.;
        _amplify = sample_rate * 2. / deviation_hz;
        _add = -(baseband_carrier_hz - deviation_hz / 4) / sample_rate;
    }

private:
    out_span_t process(in_span_t const& in) override
    {
        auto data = _float2complex.process(in);
        auto const out_start = reinterpret_cast<float*>(data.data());
        auto out = out_start;

        for (auto& s : data) {
            auto prev = s;
            _prev_sample.imag(-_prev_sample.imag());
            s = s * _prev_sample;
            _prev_sample = prev;

            /*if ((s.real() == 0) || (s.imag() == 0)) {
                *out++ = 0.;
                continue;
            }*/

            *out++ = _amplify * (_add + atan2f(s.imag(), s.real()) / DSP_2PI);
            //*out++ = std::atan2f(s.imag(), s.real()) / DSP_2PI;
        }

        // conjugate

        return { out_start, data.size() };
    }
};
} // namespace

std::unique_ptr<dsp::processor<float>> make_demodulator(options::in_data_type inputType,
                                                        double sample_rate,
                                                        double baseband_carrier_hz)
{
    assert(inputType == options::in_data_type::fm);
    return std::make_unique<demodulator>(inputType, sample_rate, baseband_carrier_hz);
}