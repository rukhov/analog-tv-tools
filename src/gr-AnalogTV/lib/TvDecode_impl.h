/* -*- c++ -*- */
/*
 * Copyright 2025 Roman Ukhov.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ANALOGTV_TVDECODE_IMPL_H
#define INCLUDED_ANALOGTV_TVDECODE_IMPL_H

#include <gnuradio/AnalogTV/TvDecode.h>

namespace gr {
namespace AnalogTV {

class TvDecode_impl : public TvDecode
{
private:
    // Nothing to declare in this block.

public:
    TvDecode_impl(double samp_rate);
    ~TvDecode_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace AnalogTV
} // namespace gr

#endif /* INCLUDED_ANALOGTV_TVDECODE_IMPL_H */
