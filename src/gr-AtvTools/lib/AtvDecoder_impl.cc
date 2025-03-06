/* -*- c++ -*- */
/*
 * Copyright 2025 Roman Ukhov <ukhov.roman@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <span>

#include "AtvDecoder_impl.h"
#include <gnuradio/io_signature.h>
#include <lib-atv-tools/decoder.h>

namespace {

using namespace gr::AtvTools;

class AtvDecoder_impl : public AtvDecoder
{
    std::vector<float> _luma_buff;

private:
    std::unique_ptr<atv::decoder> _decoder;

public:
    AtvDecoder_impl(uint64_t samp_rate, int standard)
        : gr::block("AtvDecoder",
                    gr::io_signature::make(
                        1 /* min inputs */, 1 /* max inputs */, sizeof(float)),
                    gr::io_signature::make(1 /* min outputs */,
                                           7 /*max outputs */,
                                           std::vector<size_t>({ sizeof(short),
                                                                 sizeof(short),
                                                                 sizeof(short),
                                                                 sizeof(float),
                                                                 sizeof(float),
                                                                 sizeof(float),
                                                                 sizeof(float) })))
    {
        std::cout << "AtvDecoder_impl: samp_rate: " << samp_rate
                  << "; standard: " << (standard ? "PAL" : "NTSC") << std::endl;

        atv::standard params = atv::decoder::make_params(atv::standard_e(standard));

        _decoder = atv::decoder::make(params, samp_rate, {});
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
        if ((int)_luma_buff.size() < noutput_items)
            _luma_buff.resize(noutput_items);

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
namespace AtvTools {
AtvDecoder::sptr AtvDecoder::make(double samp_rate, int standard)
{
    return gnuradio::make_block_sptr<AtvDecoder_impl>(samp_rate, standard);
}
} /* namespace AtvTools */
} /* namespace gr */
