#pragma once

#include <lib-dsp/comb_feedback.h>
#include <lib-dsp/comb_feedforward.h>
#include <lib-dsp/delay.h>
#include <lib-dsp/iir_high_pass.h>
#include <lib-dsp/iir_low_pass.h>
#include <lib-dsp/pll.h>
#include <lib-dsp/sine_gen.h>
#include <lib-dsp/utils.h>

#include "color.h"
#include "cvbs.h"
#include "standard.h"

namespace atv {
class color_extractor
{
public:
    virtual ~color_extractor() = default;
    virtual void process(std::span<float const> const& chroma,
                         std::span<uint32_t> const& tags,
                         std::span<YUV> const& out_buff) = 0;
};

std::unique_ptr<color_extractor> make_color_extractor(standard const& standard,
                                                      uint64_t samp_rate)
{
    return {};
}

class color_decoder
{
    const double _burst_shift;
    dsp::iir_high_pass _high_pass;
    std::unique_ptr<color_extractor> _color_extractor;
    dsp::delay<float> _luma_delay;
    dsp::comb_feedforward _luma_comb_filter;
    bool _sync = false;
    std::vector<YUV> _buffer;

public:
    color_decoder(standard const& standard, uint64_t samp_rate)
        : _burst_shift(standard.standard == standard_e::NTSC ? -(123. / 360.)
                                                             : 135. / 360.),
          _high_pass(dsp::hz2rel_frq(
              samp_rate,
              (standard.chroma_subcarrier1_hz - standard.chroma_band_width_hz / 2))),
          _luma_comb_filter(8),
          _luma_delay(dsp::usec2samples(samp_rate, .6)),
          _color_extractor(make_color_extractor(standard, samp_rate))
    {
    }

    YUV state() const { return {}; }

    std::span<YUV> process(std::span<float const> const& inCVBS,
                           std::span<uint32_t> const& tags)
    {
        if (_buffer.size() < inCVBS.size())
            _buffer.resize(inCVBS.size());

        auto chroma_buff = _high_pass.process(inCVBS);
        auto luma_filtered_buff = _luma_comb_filter.process(inCVBS);

        if (_color_extractor)
            _color_extractor->process({ chroma_buff.data(), size_t(inCVBS.size()) },
                                      tags,
                                      { _buffer.data(), inCVBS.size() });
        else {
            for (size_t i = 0; i < luma_filtered_buff.size(); ++i) {
                _buffer[i].y = luma_filtered_buff[i];
            }
        }


        auto cvbs = inCVBS.data();
        auto cvbs_end = cvbs + inCVBS.size();
        auto out = _buffer.data();
        auto luma = luma_filtered_buff.data();

        for (; cvbs != cvbs_end; ++cvbs, ++out, ++luma) {

            out->y = _luma_delay.process(*luma);
            auto rgb = atv::Yuv2Rgb(*out);
            *out = Rgb2Yuv(rgb.fix());
        }

        return { _buffer.data(), inCVBS.size() };
    }
};

} // namespace atv