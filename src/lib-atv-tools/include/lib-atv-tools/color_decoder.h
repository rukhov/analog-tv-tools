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
#include "secam_color_extractor.h"
#include "standard.h"

namespace atv {

std::unique_ptr<color_extractor> make_color_extractor(standard const& standard,
                                                      uint64_t samp_rate)
{
    if (standard.standard == standard_e::SECAM) {
        return std::make_unique<secam_color_extractor>(samp_rate);
    }

    return {};
}

class color_decoder
{
    const double _burst_shift;
    std::unique_ptr<color_extractor> _color_extractor;
    dsp::delay<float> _luma_delay;
    bool _sync = false;
    std::vector<YUV> _buffer;

public:
    color_decoder(standard const& standard, uint64_t samp_rate)
        : _burst_shift(standard.standard == standard_e::NTSC ? -(123. / 360.)
                                                             : 135. / 360.),
          _luma_delay(dsp::usec2samples(samp_rate, .7)),
          _color_extractor(make_color_extractor(standard, samp_rate))
    {
    }

    YUV state() const { return {}; }

    std::span<YUV> process(std::span<const float> const& inCVBS,
                           std::span<uint32_t> const& tags)
    {
        if (_buffer.size() < inCVBS.size())
            _buffer.resize(inCVBS.size());

        // auto chroma_buff = _high_pass.process(inCVBS);
        // auto luma_filtered_buff = _luma_low_pass.process(inCVBS);

        assert(_color_extractor);

        _color_extractor->process(inCVBS, tags, { _buffer.data(), inCVBS.size() });

        auto cvbs = inCVBS.data();
        auto cvbs_end = cvbs + inCVBS.size();
        auto out = _buffer.data();
        // auto luma = luma_filtered_buff.data();

        for (; cvbs != cvbs_end; ++cvbs, ++out) {

            out->y = _luma_delay.process(out->y);
            // out->u = 0; out->v = 0;
            //   out->y = *luma;
        }

        return { _buffer.data(), inCVBS.size() };
    }
};

} // namespace atv