/* -*- c++ -*- */
/*
 * Copyright 2025 Roman Ukhov.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ANALOGTV_TVDECODE_H
#define INCLUDED_ANALOGTV_TVDECODE_H

#include <gnuradio/AnalogTV/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace AnalogTV {

/*!
 * \brief <+description of block+>
 * \ingroup AnalogTV
 *
 */
class ANALOGTV_API TvDecode : virtual public gr::block
{
public:
    typedef std::shared_ptr<TvDecode> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of AnalogTV::TvDecode.
     *
     * To avoid accidental use of raw pointers, AnalogTV::TvDecode's
     * constructor is in a private implementation
     * class. AnalogTV::TvDecode::make is the public interface for
     * creating new instances.
     */
    static sptr make(double samp_rate);
};

} // namespace AnalogTV
} // namespace gr

#endif /* INCLUDED_ANALOGTV_TVDECODE_H */
