#include "../lib-dsp/resampler.h"
#include <soxr.h>

namespace {

void* create_resampler(double isr, double osr)
{
    soxr_error_t err = 0;
    auto fmt = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
    // auto auto qlt = soxr_quality_spec(SOXR_HQ, 0);
    //  auto qlt = soxr_quality_spec(SOXR_QQ, SOXR_16_BITQ | SOXR_LSR2Q);
    auto qlt = soxr_quality_spec(SOXR_LQ, 0);
    auto res = soxr_runtime_spec(4);

    auto ret_val =
        soxr_create(isr, /* Input sample-rate. */
                    osr, /* Output sample-rate. */
                    1,   /* Number of channels to be used. */
                    /* All following arguments are optional (may be set to NULL). */
                    &err, /* To report any error during creation. */
                    &fmt, /* To specify non-default I/O formats. */
                    &qlt, /* To specify non-default resampling quality.*/
                    &res  /* To specify non-default runtime resources.*/

        );

    return ret_val;
}
} // namespace

namespace dsp {

resampler::resampler(double isr, double osr)
    : _resampler(create_resampler(isr, osr)), _ratio(osr / isr), _buff(1024)
{
}
resampler::~resampler() { soxr_delete((soxr_t)_resampler); }

std::span<float> resampler::process(std::span<float> const& data)
{
    size_t inIdx = 0;
    size_t outIdx = 0;

    for (;;) {

        if (inIdx == data.size())
            break;

        if (_buff.size() < (data.size() - inIdx) * _ratio) {
            _buff.resize((data.size() - inIdx) * _ratio);
        }

        size_t odone = 0;
        size_t idone = 0;
        auto err = soxr_process(
            (soxr_t)_resampler,    /* As returned by soxr_create. */
                                   /* Input (to be resampled): */
            data.data() + inIdx,   /* Input buffer(s); may be NULL (see below). */
            data.size() - inIdx,   /* Input buf. length (samples per channel). */
            &idone,                /* To return actual # samples used (<= ilen). */
                                   /* Output (resampled): */
            _buff.data() + outIdx, /* Output buffer(s).*/
            _buff.size() - outIdx, /* Output buf. length (samples per channel). */
            &odone);
        /* To return actual # samples out (<= olen). */

        if (err)
            throw std::runtime_error("Resampler error.");

        inIdx += idone;
        outIdx += odone;
    }

    return { _buff.data(), outIdx };
}
} // namespace dsp