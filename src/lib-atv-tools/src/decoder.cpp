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


#include "../include/lib-atv-tools/color_decoder.h"
#include "../include/lib-atv-tools/decoder.h"
#include "../include/lib-atv-tools/pulse_detector.h"
#include "../include/lib-atv-tools/pulse_detector2.h"

#include <lib-dsp/cross_correlation.h>
#include <lib-dsp/iir_low_pass.h>
#include <lib-dsp/limiter.h>
#include <lib-dsp/trigger.h>
#include <lib-dsp/type_convertor.h>
#include <lib-dsp/utils.h>

namespace {

using namespace atv;

class decoder_impl : public decoder
{
    color_decoder _color_decoder;
    pulse_detector2 _pulse_detector;
    video_buffer _video_buffer;

    dsp::type_convertor<float, int64_t, 1000.f> _integer_cvbs;
    dsp::cross_correlation<float> _pulse_correlation;
    dsp::trigger<float> _vsync_trigger;

public:
    decoder_impl(standard const& params,
                 uint64_t samp_rate,
                 bool black_and_white,
                 video_buffer::frame_cb& frame_cb)
        : _pulse_detector(params, samp_rate),
          _color_decoder(params, samp_rate, black_and_white),
          _video_buffer(params, samp_rate, frame_cb),
          _pulse_correlation(
              dsp::usec2samples(samp_rate, params.vertical_serration_pulse_length_us)),
          _vsync_trigger(-.25 * dsp::usec2samples(
                                    samp_rate, params.vertical_serration_pulse_length_us),
                         false)
    {
    }

private:
    // decoder
    void process(uint64_t length,
                 float const* binput,
                 short* video_yout /*=nullptr*/,
                 short* video_uout /*=nullptr*/,
                 short* video_vout /*=nullptr*/,
                 float* luma_out /*=nullptr*/,
                 float* dbg1 /*=nullptr*/,
                 float* dbg2 /*=nullptr*/,
                 float* dbg3 /*=nullptr*/
                 ) override
    {
        if (!length)
            return;

        // auto int_cvbs = _integer_cvbs.process({ binput, length });

        auto in = binput;
        auto yout = video_yout;
        auto uout = video_uout;
        auto vout = video_vout;
        auto lout = luma_out;
        auto d1out = dbg1;
        auto d2out = dbg2;
        auto d3out = dbg3;

        auto tags = _pulse_detector.process({ binput, length });
        auto pulses =
            _vsync_trigger.process(_pulse_correlation.process({ binput, length }));
        // auto pulses = std::span<int>(int_cvbs);

        auto yuv_buff =
            _color_decoder.process(std::span<float const>(binput, length), tags);

        auto yuv = yuv_buff.data();
        auto tag = tags.data();

        for (auto i = 0; i < length; ++i,
                  ++in,
                  ++yuv,
                  ++lout,
                  ++d1out,
                  ++d2out,
                  ++d3out,
                  ++yout,
                  ++uout,
                  ++vout,
                  ++tag) {

            auto pixel = _video_buffer.process(*yuv, *tag);

            if (video_yout && video_uout && video_vout) {
                *yout = pixel.Y;
                *uout = pixel.Cb;
                *vout = pixel.Cr;
            }

            if (luma_out) {
                *lout = yuv->y;
                //*lout = *in;
            }

            if (dbg1) {
                *d1out = yuv->u;
                //*bout2 = 0;
                //*d1out = (*tag) & (cvbs_tag::vsync_odd | cvbs_tag::vsync_even);
                //*d1out = pd_debug1;
                //*dout = _luma_low_pass.state();
                //*dout = -_sync_pulse_detector.state();
                //*dout = _vsync_integrator.state();
                //*dout = _vsync_dc_eliminator.state();
            }

            if (dbg2) {
                *d2out = yuv->v;
                //*d2out = (*tag) & cvbs_tag::color_burst;
            }

            if (dbg3) {
                //*d3out = *tag;
                *d3out = pulses[i];
                //*d3out = int_cvbs[i];
            }
        }
    }
};

} // namespace

namespace atv {

std::unique_ptr<decoder> decoder::make(standard const& params,
                                       uint64_t samp_rate,
                                       bool black_and_white,
                                       video_buffer::frame_cb frame_cb)
{
    return std::make_unique<decoder_impl>(params, samp_rate, black_and_white, frame_cb);
}
} // namespace atv