#pragma once

#include "color.h"
#include "color_extractor.h"
#include "cvbs.h"
#include "standard.h"

#include <lib-dsp/comb_feedback.h>
#include <lib-dsp/delay.h>
#include <lib-dsp/fir.h>
#include <lib-dsp/fm_demodulator.h>
#include <lib-dsp/iir.h>
#include <lib-dsp/iir_band_pass.h>
#include <lib-dsp/iir_low_pass.h>
#include <lib-dsp/low_pass_1st.h>
#include <lib-dsp/quadrature_demod.h>
#include <lib-dsp/utils.h>

namespace {
constexpr double c_heterodine_frq = .25;
constexpr double c_u_deviation = 280000.;
constexpr double c_v_deviation = 230000.;
} // namespace

namespace atv {

class secam_color_extractor : public color_extractor
{
    std::unique_ptr<dsp::processor<float>> _color_band_pass;
    std::unique_ptr<dsp::processor<float>> _color_hilbert_transform;
    std::unique_ptr<dsp::processor<float>> _color_demodulator;
    std::unique_ptr<dsp::processor<float, std::complex<float>>> _quad_demodulator;
    const double _u_color_value_offset;
    const double _v_color_value_offset;
    const double _u_color_value_scale;
    const double _v_color_value_scale;
    dsp::comb_feedback _u_comb_feedback;
    dsp::comb_feedback _v_comb_feedback;
    std::vector<float> _color_buffer;
    std::unique_ptr<dsp::processor<float>> _uv_low_pass;
    std::unique_ptr<dsp::processor<float>> _luma_low_pass;
    dsp::delay<float> _line_delay;
    bool _odd_line = true;

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
        : _color_band_pass(dsp::make_band_pass<float, 5>(
              (calc_chroma_band_center()) / samp_rate,
              (_standard.chroma_band_width_hz) / samp_rate)),
          _color_demodulator(dsp::make_fm_demodulator<float, float>()),
          //_quad_demodulator(
          // dsp::make_quadrature_demod<float>((calc_chroma_band_center()) / samp_rate)),
          _quad_demodulator(dsp::make_quadrature_demod<float>(.25)),
          _u_color_value_offset(
              c_heterodine_frq +
              ((_standard.chroma_subcarrier2_hz - c_u_deviation) / samp_rate)),
          _v_color_value_offset(
              c_heterodine_frq +
              ((_standard.chroma_subcarrier1_hz - c_v_deviation) / samp_rate)),
          _u_color_value_scale(samp_rate / c_u_deviation / 4),
          _v_color_value_scale(samp_rate / c_v_deviation / 4),
          _uv_low_pass(dsp::make_low_pass<float, 1>(130000. / samp_rate)),
          _color_hilbert_transform(dsp::make_fir_hilbert_transform<float>(15)),
          _line_delay(dsp::usec2samples(samp_rate, _standard.H_us)),
          _u_comb_feedback(2),
          _v_comb_feedback(2),
          _luma_low_pass(dsp::make_band_stop<float, 3>(
              (calc_chroma_band_center()) / samp_rate,
              (_standard.chroma_band_width_hz + 1000000) / samp_rate))
    {
    }
    // color_extractor
private:
    void process(std::span<const float> const& chroma,
                 std::span<uint32_t> const& tags,
                 std::span<YUV> const& out_buff) override
    {
        if (_color_buffer.size() < chroma.size())
            _color_buffer.resize(chroma.size());

        auto luma = _luma_low_pass->process(chroma);


        auto u = _color_band_pass->process(chroma);
        auto quad = _quad_demodulator->process(u);
        // auto quad_filtered = _quad_high_pass->process(quad);
        //  auto v = _color_hilbert_transform->process(u);

        // u = _color_demodulator->process(u);
        //     v = _v_demodulator->process(v);
        //  u = _u_low_pass.process(u);
        //     v = _v_low_pass.process(v);
        //      u = _u_comb_feedback.process(u);
        //      v = _v_comb_feedback.process(v);

        for (auto i = 0; i < chroma.size(); ++i) {
            _color_buffer[i] = (quad[i].real());
        }

        auto uv = _uv_low_pass->process({ _color_buffer.data(), chroma.size() });

        for (auto i = 0; i < chroma.size(); ++i) {

            float u, v;
            auto delayed = _line_delay.process(uv[i]);

            if ((tags[i] & cvbs_tag::hsync) == cvbs_tag::hsync) {
                _odd_line = !_odd_line;
            }

            if (_odd_line) {
                u = uv[i];
                v = delayed;
            } else {
                u = delayed;
                v = uv[i];
            }

            out_buff[i].y = luma[i];
            out_buff[i].u = (u - _u_color_value_offset) * _u_color_value_scale;
            out_buff[i].v = (v - _v_color_value_offset) * _v_color_value_scale;
            // out_buff[i].v = v[i];
            //  out_buff[i].u = -.5;
            //  out_buff[i].v = .5;
        }
    }
};
} // namespace atv