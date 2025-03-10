#include "../include/lib-dsp/iir.h"

#include <Iir.h>

namespace {

template <typename T, size_t order>
class iir_impl : public dsp::processor<T, T>
{
    Iir::Butterworth::BandPass<order> _f;
    std::vector<float> _buffer;

public:
    using out_span_t = dsp::processor<T, T>::out_span_t;
    using in_span_t = dsp::processor<T, T>::in_span_t;

    iir_impl(double centerFrequencyNormalised, double bandWidthNormalised)
    {
        _f.setupN(centerFrequencyNormalised, bandWidthNormalised);
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
} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, float>> make_band_pass<float>(
    size_t order, double centerFrequencyNormalised, double bandWidthNormalised)
{
    switch (order) {
    case 1:
        return std::make_unique<iir_impl<float, 1>>(centerFrequencyNormalised,
                                                    bandWidthNormalised);
    case 2:
        return std::make_unique<iir_impl<float, 2>>(centerFrequencyNormalised,
                                                    bandWidthNormalised);
    case 3:
        return std::make_unique<iir_impl<float, 3>>(centerFrequencyNormalised,
                                                    bandWidthNormalised);
    case 4:
        return std::make_unique<iir_impl<float, 4>>(centerFrequencyNormalised,
                                                    bandWidthNormalised);
    case 5:
        return std::make_unique<iir_impl<float, 5>>(centerFrequencyNormalised,
                                                    bandWidthNormalised);
    default:
        throw std::runtime_error(std::format("Unsupported filter order: {}", order));
    }
}
} // namespace dsp