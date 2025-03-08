#pragma once

#include "color_extractor.h"

#include <lib-dsp/comb_feedback.h>
#include <lib-dsp/fm_demodulator.h>
#include <lib-dsp/iir_band_pass.h>
#include <lib-dsp/low_pass_1st.h>

namespace atv {

class secam_color_extractor : public color_extractor
{
    dsp::iir_band_pass _band_pass;
    std::unique_ptr<dsp::processor<float>> _fm_demodulator;
    dsp::comb_feedback _comb_feedback;
    dsp::low_pass_1st<float> _low_pass;

public:
    secam_color_extractor(double samp_rate)
        : _band_pass(.2, .3),
          _fm_demodulator(dsp::make_fm_demodulator<float, float>()),
          _comb_feedback(2),
          _low_pass(samp_rate, 500000.)
    {
    }
    // color_extractor
private:
    void process(std::span<float const> const& chroma,
                 std::span<uint32_t> const& tags,
                 std::span<YUV> const& out_buff) override
    {
        auto u = _band_pass.process(chroma);

        auto demod = _fm_demodulator->process(u);
        _low_pass.process_inplace(demod);
        // auto comb = _comb_feedback.process(demod);

        for (auto i = 0; i < chroma.size(); ++i) {
            out_buff[i].u = demod[i];
            // out_buff[i].u = -.5;
            // out_buff[i].v = .5;
        }
    }
};
} // namespace atv