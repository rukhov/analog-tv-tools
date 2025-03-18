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

#include <lib-dsp/contraphase_detector.h>
#include <lib-dsp/differentiator.h>
#include <lib-dsp/integrator.h>
#include <lib-dsp/pll.h>
#include <lib-dsp/pulse_detector.h>
#include <lib-dsp/trigger.h>
#include <lib-dsp/unovibrator.h>
#include <lib-dsp/utils.h>

#include "cvbs.h"
#include "standard.h"

namespace atv {
class pulse_detector2
{
public:
    pulse_detector2(standard const& standard, double samp_rate)
        : _H_length(dsp::usec2samples(samp_rate, standard.H_us)),
          _H_div_2_length(_H_length / 2),
          _lines_per_frame(standard.total_line_count / 2 - 1),
          _vsync_trigger(.25, true),
          _hsync_pll(dsp::pll::make_regular(
              dsp::hz2rel_frq(samp_rate, standard.hsync_pulse_frequency_hz),
              dsp::hz2rel_frq(samp_rate, standard.hsync_pulse_frequency_hz) / 100)),
          _hsync_trigger(.738, true),
          _sync_pulse_detector(
              dsp::usec2samples(samp_rate,
                                standard.pre_equalisation_pulse_length_us / 2 * .7),
              .2,
              .01),
          _vsync_integrator(dsp::relTimeSec(samp_rate, .0001)),
          _vsync_dc_eliminator(
              dsp::sec2samples(samp_rate, 1 / standard.vsync_pulse_frequency_hz)),
          _hsync_pulse_min_length(dsp::usec2samples(
              samp_rate, standard.hsync_pulse_length_us - standard.timing_tolerance_us)),
          _pre_equalisation_pulse_min_length(dsp::usec2samples(
              samp_rate,
              standard.pre_equalisation_pulse_length_us - standard.timing_tolerance_us)),
          _vertical_serration_pulse_min_length(
              dsp::usec2samples(samp_rate,
                                standard.vertical_serration_pulse_length_us -
                                    standard.timing_tolerance_us)),
          _vertical_serration_pulse_max_length(
              dsp::usec2samples(samp_rate,
                                standard.vertical_serration_pulse_length_us +
                                    standard.timing_tolerance_us)),
          _hline_min_length(dsp::usec2samples(samp_rate,
                                              standard.H_us -
                                                  standard.hsync_pulse_length_us -
                                                  standard.timing_tolerance_us)),
          _hline_max_length(
              dsp::usec2samples(samp_rate,
                                1000000 / standard.hsync_pulse_frequency_hz -
                                    standard.hsync_pulse_length_us +
                                    standard.pre_equalisation_pulse_length_us)),
          _color_burst_sync(
              dsp::usec2samples(samp_rate,
                                standard.h_ref_point_to_burst_styart_us -
                                    standard.hsync_pulse_length_us),
              dsp::usec2samples(samp_rate, standard.chroma_burst_duration_us)),
          _hsync(dsp::usec2samples(samp_rate, 1.35), 1)
    {
    }

    void reset_pulses()
    {
        _even_frame = _cycles_since_hline % _H_length < _H_div_2_length;
        // std::cout << _hsync_line_counter << ".";
        _hsync_line_counter = 0;
    }

    bool event_frame() const { return _even_frame; }

    std::span<uint32_t> const process(std::span<float const> const& in_cvbs)
    {
        if (_tags_buff.size() < in_cvbs.size())
            _tags_buff.resize(in_cvbs.size());

        auto const* cvbs = in_cvbs.data();
        auto* tag = _tags_buff.data();
        auto const* cvbs_end = cvbs + in_cvbs.size();

        for (; cvbs != cvbs_end; ++cvbs, ++tag) {

            _sync_pulse_detector.process(-(*cvbs));
            count_pulses();

            if (_hsync_line_counter > 7 && _hsync_line_counter < _lines_per_frame - 22) {
                _hsync_pll->process(_sync_pulse_detector.state());
            } else {
                _hsync_pll->process(0);
            }

            _hsync_trigger.process(_hsync_pll->phase());

            _vsync_integrator.process(_sync_pulse_detector.state());
            _vsync_dc_eliminator.process(_vsync_integrator.state());
            _vsync_trigger.process(_vsync_dc_eliminator.state());

            if (_vsync_trigger.state() > 0 && _hsync_line_counter == 0)
                _vsync_trigger.reset(-1);

            if (_vsync_trigger.state() > 0) {
                reset_pulses();
            }

            _hsync.process(_hsync_trigger.state() > 0 ? 1 : -1);
            _color_burst_sync.process(_hsync.state());

            *tag = 0;

            if (_vsync_trigger.state() > 0)
                *tag |= _even_frame ? cvbs_tag::vsync_even : cvbs_tag::vsync_odd;

            if (_hsync.state() > 0)
                *tag |= cvbs_tag::hsync;

            if (_color_burst_sync.state() > 0)
                *tag |= cvbs_tag::color_burst;
        }

        return { _tags_buff.data(), in_cvbs.size() };
    }

private:
    void count_pulses()
    {
        bool s = _sync_pulse_detector.state() > 0;

        if (s) {

            // pulse raise
            if (!_cycles_since_pulse_raise) {

                // regular line
                if (_cycles_since_pulse_fall > _hline_min_length &&
                    _cycles_since_pulse_fall <= _hline_max_length) {

                    ++_hsync_line_counter;
                    _cycles_since_hline = 0;
                    _hline = false;
                }

                _cycles_since_pulse_fall = 0;
            }

            ++_cycles_since_pulse_raise;
        }

        if (!s) {

            _hline = true;
            ++_cycles_since_hline;

            // pulse fall
            if (!_cycles_since_pulse_fall) {

                // ?
                if (_cycles_since_pulse_raise >= _vertical_serration_pulse_max_length) {
                } else if (_cycles_since_pulse_raise >=
                           _vertical_serration_pulse_min_length) {
                    // vertical serratioin pulse

                } else if (_cycles_since_pulse_raise >= _hsync_pulse_min_length) {
                    // hsync pulse

                } else if (_cycles_since_pulse_raise >=
                           _pre_equalisation_pulse_min_length) {
                    // pre-equalisation pulse

                } else {
                    // unknown pulse
                }
                _cycles_since_pulse_raise = 0;
            }

            ++_cycles_since_pulse_fall;
        }
    }

private:
    const uint64_t _H_length;
    const uint64_t _H_div_2_length;

    const uint64_t _hsync_pulse_min_length;
    const uint64_t _pre_equalisation_pulse_min_length;
    const uint64_t _vertical_serration_pulse_min_length;
    const uint64_t _vertical_serration_pulse_max_length;
    const uint64_t _hline_min_length;
    const uint64_t _hline_max_length;
    const uint64_t _lines_per_frame;

    dsp::pulse_detector<float> _sync_pulse_detector;
    std::unique_ptr<dsp::pll> _hsync_pll;
    dsp::trigger<float> _hsync_trigger;
    dsp::integrator<float> _vsync_integrator;
    dsp::differentiator<float> _vsync_dc_eliminator;
    dsp::trigger<float> _vsync_trigger;
    dsp::unovibrator<int8_t> _hsync;
    dsp::unovibrator<int8_t> _color_burst_sync;

    bool _even_frame = false;

    uint64_t _cycles_since_pulse_raise = 0;
    uint64_t _cycles_since_pulse_fall = 0;
    uint64_t _cycles_since_hline = 0;
    bool _hline = false;

    uint64_t _hsync_line_counter = 0;

    std::vector<uint32_t> _tags_buff;
};

} // namespace atv