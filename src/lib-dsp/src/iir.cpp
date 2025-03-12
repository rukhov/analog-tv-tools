#include "../include/lib-dsp/iir.h"

#include <Iir.h>

namespace {

template <typename Filter, typename T>
class iir_impl : public dsp::processor<T, T>
{
    Filter _f;
    std::vector<float> _buffer;

public:
    using out_span_t = dsp::processor<T, T>::out_span_t;
    using in_span_t = dsp::processor<T, T>::in_span_t;

    iir_impl(Filter&& f) : _f(std::move(f))
    {
        //_f.setupN(centerFrequencyNormalised, bandWidthNormalised);
    }

    // processor
private:
    T process(T s) override { return _f.filter(s); };

    out_span_t process(in_span_t const& in) override
    {

        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        for (size_t i = 0; i < in.size(); ++i) {
            _buffer[i] = _f.filter(in[i]);
        }

        return { _buffer.data(), in.size() };
    }
};

template <typename Filter>
class iir_impl<Filter, std::complex<float>>
    : public dsp::processor<std::complex<float>, std::complex<float>>
{
    Filter _real;
    Filter _imag;
    std::vector<std::complex<float>> _buffer;

public:
    using out_span_t =
        dsp::processor<std::complex<float>, std::complex<float>>::out_span_t;
    using in_span_t = dsp::processor<std::complex<float>, std::complex<float>>::in_span_t;

    iir_impl(Filter&& f) : _real(std::move(f)), _imag(_real) {}

    // processor
private:
    std::complex<float> process(std::complex<float> s) override
    {
        std::complex<float> out;
        out.real(_real.filter(s.real()));
        out.imag(_imag.filter(s.imag()));
        return out;
    }

    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        for (size_t i = 0; i < in.size(); ++i) {
            _buffer[i].real(_real.filter(in[i].real()));
            _buffer[i].imag(_imag.filter(in[i].imag()));
        }

        return { _buffer.data(), in.size() };
    }
};

template <typename FilterType, typename DataType, typename... Args>
std::unique_ptr<iir_impl<FilterType, DataType>> make_filter(Args&&... args)
{
    FilterType f;
    f.setupN(std::forward<Args>(args)...);
    return std::make_unique<iir_impl<FilterType, DataType>>(std::move(f));
}

} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, float>>
make_band_pass<float, 1>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandPass<1>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_pass<float, 2>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandPass<2>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_pass<float, 3>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandPass<3>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_pass<float, 4>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandPass<4>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_pass<float, 5>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandPass<5>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

} // namespace dsp

namespace dsp {

// band stop

template <>
std::unique_ptr<processor<float, float>>
make_band_stop<float, 1>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandStop<1>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_stop<float, 2>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandStop<2>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_stop<float, 3>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandStop<3>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_band_stop<float, 4>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandStop<4>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}


template <>
std::unique_ptr<processor<float, float>>
make_band_stop<float, 5>(double centerFrequencyNormalised, double bandWidthNormalised)
{
    return make_filter<Iir::Butterworth::BandStop<5>, float>(centerFrequencyNormalised,
                                                             bandWidthNormalised);
}


} // namespace dsp

namespace dsp {

// High pass

template <>
std::unique_ptr<processor<std::complex<float>, std::complex<float>>>
make_high_pass<std::complex<float>, 3>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::HighPass<3>, std::complex<float>>(
        cutoffFrequencyNormalised);
}

template <>
std::unique_ptr<processor<std::complex<float>, std::complex<float>>>
make_high_pass<std::complex<float>, 5>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::HighPass<5>, std::complex<float>>(
        cutoffFrequencyNormalised);
}

} // namespace dsp

namespace dsp {

// Low pass

template <>
std::unique_ptr<processor<float, float>>
make_low_pass<float, 1>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::LowPass<1>, float>(cutoffFrequencyNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_low_pass<float, 2>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::LowPass<2>, float>(cutoffFrequencyNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_low_pass<float, 3>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::LowPass<3>, float>(cutoffFrequencyNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_low_pass<float, 4>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::LowPass<4>, float>(cutoffFrequencyNormalised);
}

template <>
std::unique_ptr<processor<float, float>>
make_low_pass<float, 5>(double cutoffFrequencyNormalised)
{
    return make_filter<Iir::Butterworth::LowPass<5>, float>(cutoffFrequencyNormalised);
}
} // namespace dsp