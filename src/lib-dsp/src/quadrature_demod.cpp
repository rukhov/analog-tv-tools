#include "../include/lib-dsp/quadrature_demod.h"
#include "../include/lib-dsp/utils.h"


#include <siglib.h>

namespace {

template <typename T>
class quadrature_demod_impl : public dsp::processor<T, std::complex<T>>
{
    static constexpr size_t SINE_TABLE_SIZE =
        1024; // Look up table for fast sine calculation

    float _sine_table[SINE_TABLE_SIZE];
    double _phase = 0;
    double _carrierFrequency;
    std::vector<std::complex<T>> _buffer;

    void add_pahse(double adder)
    {
        _phase += adder;
        _phase = dsp::normalize_phase(_phase);
    }

    T getSine(double p) const
    {
        return std::sin(p * dsp::DSP_2PI);

        size_t idx = (size_t)((p * ((double)SINE_TABLE_SIZE)));

        if (idx >= SINE_TABLE_SIZE) {
            return 0;
        }

        SLData_t Dst = _sine_table[idx];

        return Dst;
    }

public:
    using out_span_t = dsp::processor<T, std::complex<T>>::out_span_t;
    using in_span_t = dsp::processor<T, std::complex<T>>::in_span_t;

    quadrature_demod_impl(double carrierFrequency) : _carrierFrequency(carrierFrequency)
    {
        SIF_FastSin(_sine_table, SINE_TABLE_SIZE);
    }

    // processor
private:
    out_span_t process(in_span_t const& in) override
    {
        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        for (size_t i = 0; i < in.size(); ++i) {

            auto s = getSine(_phase);
            auto c = getSine(dsp::normalize_phase(_phase + .25));
            add_pahse(_carrierFrequency);

            _buffer[i].real(c * in[i]);
            _buffer[i].imag(s * in[i]);
        }

        return { _buffer.data(), in.size() };
    }
};
} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, std::complex<float>>>
make_quadrature_demod(double currierFrequency)
{
    return std::make_unique<quadrature_demod_impl<float>>(currierFrequency);
}

} // namespace dsp