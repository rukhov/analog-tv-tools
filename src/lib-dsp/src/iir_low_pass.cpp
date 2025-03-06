#include "../include/lib-dsp/iir_low_pass.h"
#include <siglib.h>


namespace {

inline std::tuple<std::vector<SLData_t>, SLFixData_t> InitCoeffs(double cutoff_frequency)
{
    // Filter Order = 8
    // Sample rate = 1.00000000000000000000
    /* Floating point coefficients */
    SLData_t CoefficientArray[] = {
        2.68563550242170323568e-01, 4.40661070853912262280e-01,
        2.68563550242170434590e-01, -5.42970689101549441880e-01,
        5.20758860439802684361e-01,

        5.72241372743282128432e-01, 7.04377139174379385267e-01,
        5.72241372743282128432e-01, -7.40982502887752025877e-03,
        8.56269709689820968102e-01,

        2.38768345621287680913e-01, 2.38768345621287680913e-01,
        0.00000000000000000000e+00, -5.22463308757424638173e-01,
        0.00000000000000000000e+00
    };

    constexpr SLFixData_t STAGE_NUMBER =
        sizeof(CoefficientArray) /
        (sizeof(CoefficientArray[0]) * SIGLIB_IIR_COEFFS_PER_BIQUAD);

    std::vector<SLData_t> dstCoefs(STAGE_NUMBER * SIGLIB_IIR_COEFFS_PER_BIQUAD);

    SLData_t gain1 =
        SDA_IirLpLpShift(CoefficientArray, // const SLData_t *, Source coefficients
                         dstCoefs.data(),  // SLData_t *, Destination coefficients
                         .25,              // const SLData_t, Original cut-off frequency
                         cutoff_frequency, // const SLData_t, Required cut-off frequency
                         1.,               // const SLData_t, Sample rate (Hz)
                         STAGE_NUMBER      // const SLArrayIndex_t Number of biquads
        );

    SLData_t gain2 = SDA_IirModifyFilterGain(
        dstCoefs.data(), CoefficientArray, cutoff_frequency / 15.33, 1., STAGE_NUMBER);

    std::copy(CoefficientArray, CoefficientArray + dstCoefs.size(), dstCoefs.begin());

    return std::make_tuple(dstCoefs, STAGE_NUMBER);
}
} // namespace

namespace dsp {

iir_low_pass::iir_low_pass(double cutoff_frequency)
{
    auto [IIRCoeffs, stage_number] = InitCoeffs(cutoff_frequency);

    IIRCoeffs_ = std::move(IIRCoeffs);
    stage_number_ = stage_number;

    FilterState_.resize((stage_number_) * 2);

    SIF_Iir(FilterState_.data(), // Pointer to filter state array
            stage_number_);      // Number of second order stages
}

float iir_low_pass::process(float s)
{
    // Apply iir filter and store filtered data
    _state = static_cast<float>(
        SDS_Iir(s,                   // const SLData_t, Input sample to be filtered
                FilterState_.data(), // SLData_t *, Pointer to filter state array
                IIRCoeffs_.data(),   // const SLData_t *, Pointer to filter coefficients
                stage_number_        // const SLArrayIndex_t) Number of biquad
                ));

    return _state;
}
std::span<float> iir_low_pass::process(std::span<float const> const& s)
{
    if (_buffer.size() < s.size())
        _buffer.resize(s.size());

    // Apply iir filter and store filtered data
    SDA_Iir(s.data(),
            _buffer.data(),
            FilterState_.data(), // SLData_t *, Pointer to filter state array
            IIRCoeffs_.data(),   // const SLData_t *, Pointer to filter coefficients
            stage_number_,       // const SLArrayIndex_t) Number of biquad
            s.size());

    return { _buffer.data(), s.size() };
}
} // namespace dsp
