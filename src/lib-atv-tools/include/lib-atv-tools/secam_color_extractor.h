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

#pragma once

#include "color.h"
#include "color_extractor.h"
#include "cvbs.h"
#include "standard.h"

#include <lib-dsp/comb_feedback.h>
#include <lib-dsp/delay.h>
#include <lib-dsp/fm_demodulator.h>
#include <lib-dsp/hilbert_demod.h>
#include <lib-dsp/iir.h>
#include <lib-dsp/iir_band_pass.h>
#include <lib-dsp/iir_low_pass.h>
#include <lib-dsp/integrator.h>
#include <lib-dsp/low_pass_1st.h>
#include <lib-dsp/quadrature_demod.h>
#include <lib-dsp/utils.h>

namespace {
constexpr double c_heterodine_frq = .25;
constexpr double c_db_deviation = 230000.;
constexpr double c_dr_deviation = 280000.;
constexpr double c_max_deviation = 350000.;
constexpr size_t c_hilbertFirLength = 127;

} // namespace

namespace atv {

class secam_color_extractor : public color_extractor
{
    const size_t _chroma_lag_length;
    double _demod_min;
    double _demod_max;
    std::unique_ptr<dsp::processor<float>> _chroma_band_pass;
    std::unique_ptr<dsp::processor<float, std::complex<float>>> _color_demodulator;
    const double _db_color_value_offset;
    const double _dr_color_value_offset;
    const double _db_color_value_scale;
    const double _dr_color_value_scale;
    dsp::comb_feedback _u_comb_feedback;
    dsp::comb_feedback _v_comb_feedback;
    std::vector<float> _color_buffer;
    std::unique_ptr<dsp::processor<float>> _chroma_de_emphasis;
    std::unique_ptr<dsp::processor<float>> _luma_band_stop;
    dsp::delay<float> _line_delay;
    dsp::delay<float> _luma_delay;
    bool _odd_line = true;
    uint64_t _component_detect_lag_samples;
    uint64_t _samples_from_hsync;
    bool _black_and_white;

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
    secam_color_extractor(double samp_rate, bool black_and_white)
        : _chroma_lag_length(c_hilbertFirLength / 2 + dsp::usec2samples(samp_rate, .4)),
          _demod_min((_standard.chroma_subcarrier1_hz - c_max_deviation) / samp_rate),
          _demod_max((_standard.chroma_subcarrier2_hz + c_max_deviation) / samp_rate),
          _color_demodulator(dsp::make_hilbert_demod<float>(c_hilbertFirLength)),
          //_chroma_band_pass(dsp::make_band_pass_chebyshev<float, 5>(
          // calc_chroma_band_center() / samp_rate, _standard.chroma_band_width_hz /
          // samp_rate, 5.)),
          //_chroma_band_pass(dsp::make_band_pass_inv_chebyshev<float,
          // 5>(calc_chroma_band_center() / samp_rate,_standard.chroma_band_width_hz /
          // samp_rate, 40.)),
          _chroma_band_pass(
              dsp::make_band_pass<float, 3>(calc_chroma_band_center() / samp_rate,
                                            _standard.chroma_band_width_hz / samp_rate)),
          //_color_demodulator(dsp::make_quadrature_demod<float>(calc_chroma_band_center()
          /// samp_rate)),
          _db_color_value_offset(((_standard.chroma_subcarrier1_hz) / samp_rate)),
          _dr_color_value_offset(((_standard.chroma_subcarrier2_hz) / samp_rate)),
          _db_color_value_scale((samp_rate / c_db_deviation) * .95),
          _dr_color_value_scale((samp_rate / c_dr_deviation) * .95),
          _chroma_de_emphasis(
              dsp::make_low_pass_inv_chebyshev<float, 1>(318000 / samp_rate, 6.)),
          _line_delay(dsp::usec2samples(samp_rate, _standard.H_us)),
          _u_comb_feedback(2),
          _v_comb_feedback(2),
          _luma_band_stop(dsp::make_band_stop<float, 2>(
              (calc_chroma_band_center()) / samp_rate,
              (_standard.chroma_band_width_hz + 100000) / samp_rate)),
          _component_detect_lag_samples(dsp::usec2samples(samp_rate, 4.)),
          _samples_from_hsync(0),
          _luma_delay(_chroma_lag_length),
          _black_and_white(black_and_white)
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

        auto luma = _luma_band_stop->process(chroma);
        // auto luma = chroma;

        if (_black_and_white) {

            for (auto i = 0; i < luma.size(); ++i) {

                out_buff[i] = YUV{ _luma_delay.process(luma[i]) * .95f, 0, 0 };
            }

            return;
        }

        auto quad = _color_demodulator->process(_chroma_band_pass->process(chroma));

        for (auto i = 0; i < chroma.size(); ++i) {
            if (quad[i].real() < _demod_min)
                _color_buffer[i] = _demod_min;
            else if (quad[i].real() > _demod_max)
                _color_buffer[i] = _demod_max;
            else
                _color_buffer[i] = (quad[i].real());
        }

        auto dbdr = _chroma_de_emphasis->process({ _color_buffer.data(), chroma.size() });

        for (auto i = 0; i < chroma.size(); ++i, ++_samples_from_hsync) {

            const auto s = dbdr[i];

            float db, dr;
            auto delayed = _line_delay.process(dbdr[i]);

            if ((tags[i] & cvbs_tag::hsync) == cvbs_tag::hsync) {
                _samples_from_hsync = 0;
            }

            if (1) {
                if (_samples_from_hsync == _component_detect_lag_samples) {

                    if (std::abs(s - _db_color_value_offset) <
                        std::abs(s - _dr_color_value_offset)) {
                        _odd_line = true;
                    } else {
                        _odd_line = false;
                    }
                }
            } else {
                if (_samples_from_hsync == _component_detect_lag_samples) {
                    _odd_line = !_odd_line;
                }
            }

            if (_odd_line) {
                db = s;
                dr = delayed;
            } else {
                db = delayed;
                dr = s;
            }

            YDbDr ydbdr;

            ydbdr.y = _luma_delay.process(luma[i]) * .95;
            ydbdr.db = (db - _db_color_value_offset) * _db_color_value_scale;
            ydbdr.dr = (dr - _dr_color_value_offset) * _dr_color_value_scale;

            out_buff[i] = Rgb2Yuv(YDbDr2Rgb(ydbdr).fix());
        }
    }
};
} // namespace atv
