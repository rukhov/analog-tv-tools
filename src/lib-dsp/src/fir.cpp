#include "../include/lib-dsp/fir.h"

#include <siglib.h>

namespace {

template <typename T>
class fir_impl : public dsp::processor<T, T>
{
    std::vector<T> _coeffs;
    std::vector<T> _state;
    std::vector<T> _buffer;
    SLArrayIndex_t _pFilterIndex;

public:
    using out_span_t = dsp::processor<T, T>::out_span_t;
    using in_span_t = dsp::processor<T, T>::in_span_t;

    fir_impl(std::vector<T>&& coeffs) : _coeffs(std::move(coeffs)), _state(_coeffs.size())
    {
        SIF_Fir(_state.data(), &_pFilterIndex, _coeffs.size());
    }

    // processor
private:
    T process(T s) override
    {
        return SDS_Fir(s, _state.data(), _coeffs.data(), &_pFilterIndex, _coeffs.size());
    };

    out_span_t process(in_span_t const& in) override
    {

        if (_buffer.size() < in.size())
            _buffer.resize(in.size());

        SDA_Fir(in.data(),
                _buffer.data(),
                _state.data(),
                _coeffs.data(),
                &_pFilterIndex,
                _coeffs.size(),
                in.size());

        return { _buffer.data(), in.size() };
    }
};
} // namespace

namespace dsp {

template <>
std::unique_ptr<processor<float, float>> make_fir_hilbert_transform<float>(size_t length)
{
    std::vector<float> coeffs(length);
    SIF_HilbertTransformerFirFilter(coeffs.data(), coeffs.size());

    return std::make_unique<fir_impl<float>>(std::move(coeffs));
}
} // namespace dsp