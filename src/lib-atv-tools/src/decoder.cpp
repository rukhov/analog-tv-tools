
#include "../include/lib-atv-tools/color_decoder.h"
#include "../include/lib-atv-tools/decoder.h"
#include "../include/lib-atv-tools/pulse_detector.h"

#include <lib-dsp/iir_low_pass.h>
#include <lib-dsp/limiter.h>
#include <lib-dsp/utils.h>

namespace {

using namespace atv;

class decoder_impl : public decoder
{
    color_decoder _color_decoder;
    pulse_detector _pulse_detector;
    video_buffer _video_buffer;

public:
    decoder_impl(standard const& params,
                 uint64_t samp_rate,
                 video_buffer::frame_cb& frame_cb)
        : _pulse_detector(params, samp_rate),
          _color_decoder(params, samp_rate),
          _video_buffer(params, samp_rate, frame_cb)
    {
    }

private:
    uint32_t make_tag(auto vsync, auto hsync)
    {
        uint32_t tag = 0;

        if (vsync > 0) {

            tag |= _pulse_detector.event_frame() ? cvbs_tag::vsync_even
                                                 : cvbs_tag::vsync_odd;
        }

        if (hsync > 0)
            tag |= cvbs_tag::hsync;

        return tag;
    }

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

        auto in = binput;
        auto yout = video_yout;
        auto uout = video_uout;
        auto vout = video_vout;
        auto lout = luma_out;
        auto d1out = dbg1;
        auto d2out = dbg2;
        auto d3out = dbg3;

        auto tags = _pulse_detector.process({ binput, length });

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

            if (dbg1) {
                //*bout2 = 0;
                //*d1out = (*tag) & (cvbs_tag::vsync_odd | cvbs_tag::vsync_even);
                //*d1out = pd_debug1;
                //*dout = _luma_low_pass.state();
                //*dout = -_sync_pulse_detector.state();
                //*dout = _vsync_integrator.state();
                //*dout = _vsync_dc_eliminator.state();
            }

            if (dbg2) {
                //*d2out = (*tag) & cvbs_tag::color_burst;
            }

            if (dbg3) {
                //*d3out = pd_debug2;
            }
        }
    }
};

} // namespace

namespace atv {

standard decoder::make_params(standard_e standard)
{

    if (standard == standard_e::NTSC) {
        constexpr auto Nline = 525;
        constexpr auto Fsc = 63. * 5000000. / 88.;
        constexpr auto Fh = 2 * Fsc / 455.;
        constexpr auto Fv = 2 * Fh / Nline;
        constexpr auto V_us = 1000000 / Fv;
        constexpr auto H_us = 1000000 / Fh;
        constexpr auto Hsc_us = 1000000 / Fsc;
        return { .standard = atv::standard_e::NTSC,
                 .total_line_count = Nline,
                 .interlaced = true,
                 .chroma_band_center_hz = Fsc,
                 .chroma_band_width_hz = 1800000.,
                 .H_us = H_us,
                 .timing_tolerance_us = 1.2,
                 .chroma_burst_duration_us = 2.5,
                 .hsync_pulse_length_us = 4.7,
                 .vsync_pulse_frequency_hz = Fv,
                 .hsync_pulse_frequency_hz = Fh,
                 .h_ref_point_to_burst_styart_us = 19 * Hsc_us,
                 .pre_equalisation_pulse_length_us = H_us * .04,
                 .vertical_serration_pulse_length_us = (H_us / 2 - H_us * .07) };
    } else if (standard == standard_e::PAL) {
        // PAL 625
        constexpr auto Nline = 625;
        constexpr auto Fh = 15625.;
        constexpr auto Fsc = (1135. / 4. + 1. / Nline) * Fh; // 4433618.75;
        constexpr auto Fv = 2 * Fh / Nline;
        constexpr auto V_us = 1000000 / Fv;
        constexpr auto H_us = 1000000 / Fh;
        constexpr auto Hsc_us = 1000000 / Fsc;
        return { .standard = atv::standard_e::PAL,
                 .total_line_count = Nline,
                 .interlaced = true,
                 .chroma_band_center_hz = Fsc,
                 .chroma_band_width_hz = 1800000.,
                 .H_us = H_us,
                 .timing_tolerance_us = 1.2,
                 .chroma_burst_duration_us = 2.25,
                 .hsync_pulse_length_us = 4.7,
                 .vsync_pulse_frequency_hz = Fv,
                 .hsync_pulse_frequency_hz = Fh,
                 .h_ref_point_to_burst_styart_us = 5.6,
                 .pre_equalisation_pulse_length_us = 2.35,
                 .vertical_serration_pulse_length_us = 27.3 };
    }
    std::unreachable();
}

std::unique_ptr<decoder>
decoder::make(standard const& params, uint64_t samp_rate, video_buffer::frame_cb frame_cb)
{
    return std::make_unique<decoder_impl>(params, samp_rate, frame_cb);
}
} // namespace atv