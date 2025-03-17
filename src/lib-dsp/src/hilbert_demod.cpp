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

#include "lib-dsp/delay.h"
#include "lib-dsp/fir.h"
#include "lib-dsp/hilbert_demod.h"
#include "lib-dsp/iir.h"
#include "lib-dsp/utils.h"


#include <siglib.h>

namespace {

using namespace dsp;

template <typename T>
class hilbert_demod_impl : public dsp::processor<T, std::complex<T>>
{
    std::vector<std::complex<T>> _buffer;
    std::vector<std::complex<T>> _out_buffer;
    std::unique_ptr<dsp::processor<float>> _hilbert_transform_fir;
    dsp::delay<float> _hilbert_delay;
    std::complex<float> _prev_sample = {};

public:
    using out_span_t = dsp::processor<T, std::complex<T>>::out_span_t;
    using in_span_t = dsp::processor<T, std::complex<T>>::in_span_t;

    hilbert_demod_impl(size_t hilbertFirLength)
        : _hilbert_transform_fir(
              dsp::make_fir_hilbert_transform<float>(hilbertFirLength)),
          _hilbert_delay((hilbertFirLength / 2) + 1)
    {
        assert(hilbertFirLength & 0x1);
    }

    // processor
private:
    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size()) {
            _buffer.resize(in.size());
            _out_buffer.resize(in.size());
        }

        auto hilbert = _hilbert_transform_fir->process(in);

        for (size_t i = 0; i < in.size(); ++i) {

            _buffer[i].real(_hilbert_delay.process(in[i]));
            _buffer[i].imag(hilbert[i]);
        }

        for (size_t i = 0; i < in.size(); ++i) {

            _out_buffer[i] = _buffer[i];
            auto prev = _buffer[i];
            _prev_sample.imag(-_prev_sample.imag());
            _buffer[i] = _buffer[i] * _prev_sample;
            _prev_sample = prev;

            _out_buffer[i].real(0. + std::atan2(_buffer[i].imag(), _buffer[i].real()) /
                                         dsp::DSP_2PI);
        }

        return { _out_buffer.data(), in.size() };
    }
};
} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, std::complex<float>>>
make_hilbert_demod(size_t hilbertFirLength)
{
    return std::make_unique<hilbert_demod_impl<float>>(hilbertFirLength);
}

} // namespace dsp