#pragma once

#include "color.h"
#include "color_extractor.h"
#include "standard.h"

#include <lib-dsp/comb_feedback.h>
#include <lib-dsp/fir.h>
#include <lib-dsp/fm_demodulator.h>
#include <lib-dsp/iir.h>
#include <lib-dsp/iir_band_pass.h>
#include <lib-dsp/iir_low_pass.h>
#include <lib-dsp/low_pass_1st.h>
#include <lib-dsp/quadrature_demod.h>


namespace atv {

class secam_color_extractor : public color_extractor
{
    std::unique_ptr<dsp::processor<float>> _color_band_pass;
    std::unique_ptr<dsp::processor<float>> _color_hilbert_transform;
    std::unique_ptr<dsp::processor<float>> _color_demodulator;
    std::unique_ptr<dsp::processor<float, std::complex<float>>> _quad_demodulator;
    dsp::comb_feedback _u_comb_feedback;
    dsp::comb_feedback _v_comb_feedback;
    dsp::iir_low_pass _u_low_pass;
    dsp::iir_low_pass _v_low_pass;

    static const atv::standard _standard;

    static inline const float calc_chroma_band_width()
    {
        auto retVal =
            std::abs(_standard.chroma_subcarrier2_hz - _standard.chroma_subcarrier1_hz);
        return retVal;
    }

    static inline const float calc_chroma_band_center()
    {
        auto retVal = (_standard.chroma_subcarrier1_hz + _standard.chroma_subcarrier2_hz);
        return retVal / 2;
    }

public:
    secam_color_extractor(double samp_rate)
        : _color_band_pass(
              dsp::make_band_pass<float>(5,
                                         (calc_chroma_band_center()) / samp_rate,
                                         (_standard.chroma_band_width_hz) / samp_rate)),
          _color_demodulator(dsp::make_fm_demodulator<float, float>()),
          _quad_demodulator(dsp::make_quadrature_demod<float>(
              0 * (calc_chroma_band_center()) / samp_rate)),
          _color_hilbert_transform(dsp::make_fir_hilbert_transform<float>(15)),
          _u_comb_feedback(2),
          _v_comb_feedback(2),
          _u_low_pass(200000. / samp_rate),
          _v_low_pass(.25)
    {
    }
    // color_extractor
private:
    void process(std::span<const float> const& chroma,
                 std::span<uint32_t> const& tags,
                 std::span<YUV> const& out_buff) override
    {
        auto u = _color_band_pass->process(chroma);
        auto quad = _quad_demodulator->process(chroma);
        // auto v = _color_hilbert_transform->process(u);

        u = _color_demodulator->process(u);
        //    v = _v_demodulator->process(v);
        // u = _u_low_pass.process(u);
        //    v = _v_low_pass.process(v);
        //     u = _u_comb_feedback.process(u);
        //     v = _v_comb_feedback.process(v);

        for (auto i = 0; i < chroma.size(); ++i) {
            out_buff[i].u = quad[i].real();
            out_buff[i].v = quad[i].imag();
            // out_buff[i].v = v[i];
            //  out_buff[i].u = -.5;
            //  out_buff[i].v = .5;
        }
    }
};
} // namespace atv