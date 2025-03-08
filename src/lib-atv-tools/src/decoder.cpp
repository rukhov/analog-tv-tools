
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

            if (luma_out) {
                *lout = yuv->y;
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

std::unique_ptr<decoder>
decoder::make(standard const& params, uint64_t samp_rate, video_buffer::frame_cb frame_cb)
{
    return std::make_unique<decoder_impl>(params, samp_rate, frame_cb);
}
} // namespace atv