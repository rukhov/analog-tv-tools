/* -*- c++ -*- */
/*
 * Copyright 2025 Roman Ukhov <ukhov.roman@gmail.com>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ATVTOOLS_ATVDECODER_H
#define INCLUDED_ATVTOOLS_ATVDECODER_H

#include <gnuradio/AtvTools/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace AtvTools {

/*!
 * \brief <+description of block+>
 * \ingroup AtvTools
 *
 */
class ATVTOOLS_API AtvDecoder : virtual public gr::block
{
public:
    typedef std::shared_ptr<AtvDecoder> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of AtvTools::AtvDecoder.
     *
     * To avoid accidental use of raw pointers, AtvTools::AtvDecoder's
     * constructor is in a private implementation
     * class. AtvTools::AtvDecoder::make is the public interface for
     * creating new instances.
     */
    static sptr make(double samp_rate, int standard);
};


} // namespace AtvTools
} // namespace gr

#endif /* INCLUDED_ATVTOOLS_ATVDECODER_H */
