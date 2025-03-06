/* -*- c++ -*- */
/*
 * Copyright 2025 Roman Ukhov.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "TvDecode_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace AnalogTV {

#pragma message("set the following appropriately and remove this warning")
using input_type = float;
#pragma message("set the following appropriately and remove this warning")
using output_type = float;
TvDecode::sptr TvDecode::make(double samp_rate)
{
    return gnuradio::make_block_sptr<TvDecode_impl>(samp_rate);
}


/*
 * The private constructor
 */
TvDecode_impl::TvDecode_impl(double samp_rate)
    : gr::block("TvDecode",
                gr::io_signature::make(
                    1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
TvDecode_impl::~TvDecode_impl() {}

void TvDecode_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
#pragma message( \
    "implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
    /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int TvDecode_impl::general_work(int noutput_items,
                                gr_vector_int& ninput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

#pragma message("Implement the signal processing in your block and remove this warning")
    // Do <+signal processing+>
    // Tell runtime system how many input items we consumed on
    // each input stream.
    consume_each(noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace AnalogTV */
} /* namespace gr */
