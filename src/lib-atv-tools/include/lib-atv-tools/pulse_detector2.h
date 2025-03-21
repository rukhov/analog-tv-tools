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
#include <lib-dsp/cross_correlation.h>
#include <lib-dsp/differentiator.h>
#include <lib-dsp/integrator.h>
#include <lib-dsp/pll.h>
#include <lib-dsp/pulse_detector.h>
#include <lib-dsp/pulse_generator.h>
#include <lib-dsp/threshold.h>
#include <lib-dsp/trigger.h>
#include <lib-dsp/unovibrator.h>
#include <lib-dsp/utils.h>

#include <boost/circular_buffer.hpp>

#include "cvbs.h"
#include "standard.h"

namespace atv {
class pulse_detector2
{
public:
    pulse_detector2(standard const& standard, double samp_rate)
        : _hline_length(dsp::usec2samples(samp_rate, standard.H_us)),
          _H_div_2_length(_hline_length / 2),

          _hpulse_correlator(
              dsp::usec2samples(samp_rate, standard.hsync_pulse_length_us)),
          _hpulse_trigger(
              -.3 * dsp::usec2samples(samp_rate, standard.hsync_pulse_length_us), false),
          _hpulse_delay(57. + 75. * samp_rate / 48000000.),
          _hsync_pulse_generator(
              samp_rate,
              dsp::hz2rel_frq(samp_rate, standard.hsync_pulse_frequency_hz),
              0),
          _vpulse_correlator(
              dsp::usec2samples(samp_rate, standard.vertical_serration_pulse_length_us)),
          _vpulse_trigger(
              -.2 * dsp::usec2samples(samp_rate,
                                      standard.vertical_serration_pulse_length_us),
              false),
          _smoothing_length(dsp::usec2samples(samp_rate, standard.H_us * 6))
    {
    }

    bool event_frame() const { return _even_frame; }

    std::span<uint32_t> const process(std::span<float const> const& in_cvbs)
    {
        if (_tags_buff.size() < in_cvbs.size())
            _tags_buff.resize(in_cvbs.size());

        auto vsync = _vpulse_trigger.process(_vpulse_correlator.process(in_cvbs));

        for (size_t i = 0; i < in_cvbs.size();
             ++i, ++_cycles_since_hpulse, ++_cycles_since_vpulse) {

            auto hsync_trigger =
                _hpulse_trigger._process(_hpulse_correlator._process(in_cvbs[i]));
            auto hsync_trigger_delay = _hpulse_delay._process(hsync_trigger);
            auto hsync = _hsync_pulse_generator._process(
                (_cycles_since_vpulse > _smoothing_length) ? hsync_trigger_delay : 0);
            // auto hsync = hsync_trigger_delay;

            _tags_buff[i] = 0;

            if (vsync[i] > 0) {

                _even_frame = _cycles_since_hpulse > _prev_cycles_since_hpulse;

                // std::cout << _cycles_since_hpulse << "-" << _prev_cycles_since_hpulse
                // << "==";

                if (_even_frame)
                    _tags_buff[i] |= cvbs_tag::vsync_even;
                else
                    _tags_buff[i] |= cvbs_tag::vsync_odd;

                _hsync_line_counter = 0;

                _prev_cycles_since_hpulse = _cycles_since_hpulse;
                _cycles_since_vpulse = 0;
            }

            if (hsync > 0) {
                _tags_buff[i] |= cvbs_tag::hsync;
                ++_hsync_line_counter;
            }
            if (hsync > 0) {
                _cycles_since_hpulse = 0;
            }
        }

        return { _tags_buff.data(), in_cvbs.size() };
    }

private:
    const uint64_t _hline_length;
    const uint64_t _H_div_2_length;

    dsp::cross_correlation<float> _hpulse_correlator;
    dsp::trigger<float> _hpulse_trigger;
    dsp::delay<float> _hpulse_delay;
    float _hpulse_correlator_min_state = 0;
    bool _hpulse_in_pulse_state = false;
    dsp::pulse_generator<float> _hsync_pulse_generator;

    // VSync
    dsp::cross_correlation<float> _vpulse_correlator;
    dsp::trigger<float> _vpulse_trigger;

    uint64_t _cycles_since_hpulse = 0;
    uint64_t _prev_cycles_since_hpulse = 0;
    uint64_t _cycles_since_vpulse = 0;
    const uint64_t _smoothing_length;

    bool _even_frame = false;

    uint64_t _hsync_line_counter = 0;

    std::vector<uint32_t> _tags_buff;
};

} // namespace atv