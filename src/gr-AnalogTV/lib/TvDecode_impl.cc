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

#include <gnuradio/AnalogTV/TvDecode.h>

#include <gnuradio/io_signature.h>

#include <lib-atv-tools/decoder.h>

namespace {

using namespace gr::AnalogTV;
using namespace atv;

class TvDecode_impl : public TvDecode
{
    std::vector<float> _luma_buffer;
    std::unique_ptr<decoder> _decoder;

public:
    TvDecode_impl(double samp_rate)
        : gr::block("TvDecode",
                    gr::io_signature::make(
                        1 /* min inputs */, 1 /* max inputs */, sizeof(float)),
                    gr::io_signature::makev(1 /* min outputs */,
                                            7 /*max outputs */,
                                            std::vector<int>({ sizeof(short),
                                                               sizeof(short),
                                                               sizeof(short),
                                                               sizeof(float),
                                                               sizeof(float),
                                                               sizeof(float),
                                                               sizeof(float) }))),
          _decoder(decoder::make(standard::make(standard_e::SECAM), samp_rate, {}))
    {
    }

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required)
    {
        // #pragma message("implement a forecast that fills in how many items on each
        // input you need to produce noutput_items and remove this warning")
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }


    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items)
    {
        if ((int)_luma_buffer.size() < noutput_items)
            _luma_buffer.resize(noutput_items);

        auto const in = static_cast<const float*>(input_items[0]);

        std::vector<float> luma(noutput_items);

        auto const video_y_bout = static_cast<short*>(output_items[0]);
        auto const video_u_bout = output_items.size() > 1
                                      ? static_cast<short*>(output_items[1])
                                      : (short*)nullptr;
        auto const video_v_bout = output_items.size() > 2
                                      ? static_cast<short*>(output_items[2])
                                      : (short*)nullptr;
        auto const luma_bout =
            output_items.size() > 3 ? static_cast<float*>(output_items[3]) : luma.data();
        auto const dbg1_bout = output_items.size() > 4
                                   ? static_cast<float*>(output_items[4])
                                   : (float*)nullptr;
        auto const dbg2_bout = output_items.size() > 5
                                   ? static_cast<float*>(output_items[5])
                                   : (float*)nullptr;
        auto const dbg3_bout = output_items.size() > 6
                                   ? static_cast<float*>(output_items[6])
                                   : (float*)nullptr;


        if (ninput_items[0] < noutput_items)
            noutput_items = ninput_items[0];

        _decoder->process(noutput_items,
                          in,
                          video_y_bout,
                          video_u_bout,
                          video_v_bout,
                          luma_bout,
                          dbg1_bout,
                          dbg2_bout,
                          dbg3_bout);

        // Tell runtime system how many input items we consumed on each input stream.
        consume_each(noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }
};

} // namespace

namespace gr {
namespace AnalogTV {
TvDecode::sptr TvDecode::make(double samp_rate)
{
    return gnuradio::make_block_sptr<TvDecode_impl>(samp_rate);
}
} // namespace AnalogTV
} // namespace gr
