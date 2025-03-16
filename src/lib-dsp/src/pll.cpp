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

#include "../include/lib-dsp/contraphase_detector.h"
#include "../include/lib-dsp/differentiator.h"
#include "../include/lib-dsp/pll.h"
#include "../include/lib-dsp/utils.h"
#include <siglib.h>

namespace {

using namespace dsp;

class pll_costas_impl : public pll
{
    static constexpr size_t COSTAS_LP_VCO_SINE_TABLE_SIZE =
        1024; // Look up table for fast sine calculation
    // static constexpr size_t COSTAS_LP_LPF_LENGTH = (((SLFixData_t)((54000000. /
    // 4430000) * 2)) | 0x1); // Costas loop LP LPF FIR filter length
    static constexpr size_t COSTAS_LP_LPF_LENGTH = 15; //
    // static constexpr double VCO_MODULATION_INDEX = 0.005; // Modulation index
    static constexpr double VCO_MODULATION_INDEX = 0.2;
    // static constexpr SLCostasLoopFeedbackMode_t FEEDBACK_MODE =
    // SIGLIB_COSTAS_LOOP_MULTIPLY_LOOP;
    static constexpr SLCostasLoopFeedbackMode_t FEEDBACK_MODE =
        SIGLIB_COSTAS_LOOP_POLARITY_LOOP;
    static constexpr double COSTAS_LP_LOOP_FILTER_ALPHA =
        0.1; // Feedback coeff for one-pole loop filter

    SLData_t* VCOLookUpTable_;
    SLData_t PLLLoopFilterState_;
    SLData_t PLLLoopFilter1State_[COSTAS_LP_LPF_LENGTH];
    SLArrayIndex_t PLLLoopFilter1Index_ = 0;
    SLData_t PLLLoopFilter2State_[COSTAS_LP_LPF_LENGTH];
    SLArrayIndex_t PLLLoopFilter2Index_ = 0;
    SLData_t PLLLoopFilterCoeffs_[COSTAS_LP_LPF_LENGTH];
    SLData_t PLLSavedSample_ = 0.; // PLL saved sample for next iteration
    SLData_t PLLVCOPhase_ = 0.;

    SLData_t carrier_frequency_ = 0.;
    float _state = 0;

public:
    pll_costas_impl(double carrier_frequency, double lpf_cut_off_frequency)
        : carrier_frequency_(carrier_frequency),
          VCOLookUpTable_(SUF_CostasLoopVCOArrayAllocate(COSTAS_LP_VCO_SINE_TABLE_SIZE))
    {
        ::SIF_CostasLoop(
            &PLLVCOPhase_,                 // SLData_t *, VCO phase
            VCOLookUpTable_,               // SLData_t , VCO fast sine look up table
            COSTAS_LP_VCO_SINE_TABLE_SIZE, // const SLArrayIndex_t, VCO fast sine look up
                                           // table size
            lpf_cut_off_frequency,         // const SLData_t, LPF cut-off frequency
            PLLLoopFilter1State_,          // SLData_t *, Pointer to loop filter 1 state
            &PLLLoopFilter1Index_, // SLArrayIndex_t *, Pointer to loop filter 1 index
            PLLLoopFilter2State_,  // SLData_t *, Pointer to loop filter 2 state
            &PLLLoopFilter2Index_, // SLArrayIndex_t *, Pointer to loop filter 2 index
            PLLLoopFilterCoeffs_,  // SLData_t *, Pointer to loop filter coefficients
            COSTAS_LP_LPF_LENGTH,  // const SLArrayIndex_t, Loop filter length
            &PLLLoopFilterState_,  // SLData_t *, Pointer to loop filter state
            &PLLSavedSample_       // SLData_t *Pointer to delayed sample
        );

        for (int i = 0; i < COSTAS_LP_VCO_SINE_TABLE_SIZE; ++i) {
            // VCOLookUpTable_[i] = (i / COSTAS_LP_VCO_SINE_TABLE_SIZE) < .5 ? 1 : 0;
            // VCOLookUpTable_[i] = 0;
        }

        // VCOLookUpTable_[0] = 1;
    }

    ~pll_costas_impl() { SUF_MemoryFree(VCOLookUpTable_); }

    // pll
private:
    float process(float s) override
    {
        _state = static_cast<float>(SDS_CostasLoop(
            s,                             // (const SLData_t, Source data
            &PLLVCOPhase_,                 // SLData_t *, VCO phase
            VCO_MODULATION_INDEX,          // const SLData_t, VCO modulation index
            VCOLookUpTable_,               // SLData_t *, VCO Fast sine look up table
            COSTAS_LP_VCO_SINE_TABLE_SIZE, // const SLArrayIndex_t, VCO Fast sine look up
                                           // table size
            carrier_frequency_,            // const SLData_t, Carrier frequency
            PLLLoopFilter1State_,          // SLData_t*, Pointer to loop filter1 state
            &PLLLoopFilter1Index_,       // SLArrayIndex_t*, Pointer to loop filter1 index
            PLLLoopFilter2State_,        // SLData_t*, Pointer to loop filter2 state
            &PLLLoopFilter2Index_,       // SLArrayIndex_t*, Pointer to loop filter2 index
            PLLLoopFilterCoeffs_,        // const SLData_t*, Pointer to loop filter
                                         // coefficients
            COSTAS_LP_LPF_LENGTH,        // const SLArrayIndex_t, Loop filter length
            &PLLLoopFilterState_,        // SLData_t*, Pointer to loop filter state
            COSTAS_LP_LOOP_FILTER_ALPHA, // const SLData_t, Loop filter coefficient
            FEEDBACK_MODE,               // Loop feedback mode
            &PLLSavedSample_             // SLData_t*, Pointer to delayed sample
            ));

        return _state;
    }

    float state() const override { return _state; }

    double phase() const override { return PLLVCOPhase_; }

    void set_phase(double p) override { PLLVCOPhase_ = normalize_phase(p); }

    void skip(uint64_t samplesCount)
    {
        for (auto i = 0; i < samplesCount; ++i)
            process(.0);
        // set_phase(phase() + carrier_frequency_ * samplesCount);
    }

    std::span<float> process(std::span<float const> const&) override { return {}; }
};

class pll_impl : public pll
{
    static constexpr size_t VCO_SINE_TABLE_SIZE =
        1024; // Look up table for fast sine calculation
    static constexpr size_t PLL_LOOP_FILTER_LENGTH = 15; // PLL loop filter length
    static constexpr size_t PLL_HILBERT_TFORM_LENGTH =
        15; // PLL Hilbert transform filter length
    static constexpr SLData_t VCO_MODULATION_INDEX = 0.2; // Modulation index

    SLData_t VCOLookUpTable_[VCO_SINE_TABLE_SIZE];
    SLData_t PLLLoopFilterState_[PLL_LOOP_FILTER_LENGTH];
    SLData_t PLLLoopFilterCoeffs_[PLL_LOOP_FILTER_LENGTH];
    SLData_t PLLHilbertTformFilterCoeffs_[PLL_HILBERT_TFORM_LENGTH];
    SLData_t pPLLHilbertTformFilterState_[PLL_HILBERT_TFORM_LENGTH];
    SLArrayIndex_t PLLLoopFilterIndex_ = 0;
    SLArrayIndex_t PLLHilbertTformFilterIndex_ = 0;
    SLData_t PLLSavedSample_ = 0.; // PLL saved sample for next iteration
    SLData_t PLLVCOPhase_ = 0.;

    float _state = {};

    SLData_t carrier_frequency_ = 0.;

    std::vector<float> _out_buff;
    std::vector<float> _out_phase;

public:
    pll_impl(double carrier_frq, double lpf_cutoff_frq) : carrier_frequency_(carrier_frq)
    {
        ::SIF_PhaseLockedLoop(
            &PLLVCOPhase_,       // SLData_t *, VCO phase
            VCOLookUpTable_,     // SLData_t *, VCO Fast sine look up table
            VCO_SINE_TABLE_SIZE, // const SLArrayIndex_t, VCO Fast sine look up table size
            lpf_cutoff_frq,      // const SLData_t, LPF cut-off frequency
            PLLLoopFilterState_, // SLData_t *, Pointer to loop filter state
            PLLLoopFilterCoeffs_, // const SLData_t *, Pointer to loop filter coefficients
            &PLLLoopFilterIndex_, // SLArrayIndex_t *, Pointer to loop filter index
            PLL_LOOP_FILTER_LENGTH,       // const SLArrayIndex_t, Loop filter length
            pPLLHilbertTformFilterState_, // SLData_t * , Pointer to Hilbert xform filter
                                          // state
            PLLHilbertTformFilterCoeffs_, // const SLData_t *, Pointer to Hilbert xform
                                          // filter coeffs
            &PLLHilbertTformFilterIndex_, // SLArrayIndex_t *, Pointer to Hilbert xform
                                          // filter index
            PLL_HILBERT_TFORM_LENGTH, // const SLArrayIndex_t, Hilbert xform filter length
            &PLLSavedSample_          // SLData_t * Pointer to delayed sample
        );
    }

    // pll
private:
    double phase() const override { return normalize_phase(PLLVCOPhase_ + .5); }
    void set_phase(double p) override { PLLVCOPhase_ = p; }
    float state() const override { return _state; }
    float process(float s) override
    {
        _state = SDS_PhaseLockedLoop(
            s,                      // (const SLData_t, Source data
            &PLLVCOPhase_,          // SLData_t *, VCO phase
            VCO_MODULATION_INDEX,   // const SLData_t, VCO modulation index
            VCOLookUpTable_,        // SLData_t *, VCO Fast sine look up table
            VCO_SINE_TABLE_SIZE,    // const SLArrayIndex_t, VCO Fast sine look up table
                                    // size
            carrier_frequency_,     // const SLData_t, Carrier frequency
            PLLLoopFilterState_,    // SLData_t*, Pointer to loop filter state
            PLLLoopFilterCoeffs_,   // const SLData_t*, Pointer to loop filter
                                    // coefficients
            &PLLLoopFilterIndex_,   // SLArrayIndex_t*, Pointer to loop filter index
            PLL_LOOP_FILTER_LENGTH, // const SLArrayIndex_t, Loop filter length
            pPLLHilbertTformFilterState_, // SLData_t*, Pointer to Hilbert xform
                                          // filter state
            PLLHilbertTformFilterCoeffs_, // const SLData_t*, Pointer to Hilbert xform
                                          // filter coeffs
            &PLLHilbertTformFilterIndex_, // SLArrayIndex_t*, Pointer to Hilbert xform
                                          // filter index const
            PLL_HILBERT_TFORM_LENGTH,     // SLArrayIndex_t, Hilbert xform filter length
            &PLLSavedSample_              // SLData_t*, Pointer to delayed sample
        );

        return state();
    }

    std::span<float> process(std::span<float const> const& in) override
    {
        if (_out_buff.size() < in.size())
            _out_buff.resize(in.size());

        SDA_PhaseLockedLoop(
            in.data(),              // Pointer to source array
            _out_buff.data(),       // Pointer to destination array
            &PLLVCOPhase_,          // SLData_t *, VCO phase
            VCO_MODULATION_INDEX,   // const SLData_t, VCO modulation index
            VCOLookUpTable_,        // SLData_t *, VCO Fast sine look up table
            VCO_SINE_TABLE_SIZE,    // const SLArrayIndex_t, VCO Fast sine look up table
                                    // size
            carrier_frequency_,     // const SLData_t, Carrier frequency
            PLLLoopFilterState_,    // SLData_t*, Pointer to loop filter state
            PLLLoopFilterCoeffs_,   // const SLData_t*, Pointer to loop filter
                                    // coefficients
            &PLLLoopFilterIndex_,   // SLArrayIndex_t*, Pointer to loop filter index
            PLL_LOOP_FILTER_LENGTH, // const SLArrayIndex_t, Loop filter length
            pPLLHilbertTformFilterState_, // SLData_t*, Pointer to Hilbert xform
                                          // filter state
            PLLHilbertTformFilterCoeffs_, // const SLData_t*, Pointer to Hilbert xform
                                          // filter coeffs
            &PLLHilbertTformFilterIndex_, // SLArrayIndex_t*, Pointer to Hilbert xform
                                          // filter index const
            PLL_HILBERT_TFORM_LENGTH,     // SLArrayIndex_t, Hilbert xform filter length
            &PLLSavedSample_,             // SLData_t*, Pointer to delayed sample
            in.size()                     // Length of input array
        );

        return { _out_buff.data(), in.size() };
    }
};

} // namespace

namespace dsp {

std::unique_ptr<pll> pll::make_costas(double carrier_frequency, double cut_off_frequency)
{
    return std::make_unique<pll_costas_impl>(carrier_frequency, cut_off_frequency);
}

std::unique_ptr<pll> pll::make_regular(double carrier_frequency, double cut_off_frequency)
{
    return std::make_unique<pll_impl>(carrier_frequency, cut_off_frequency);
}

} // namespace dsp