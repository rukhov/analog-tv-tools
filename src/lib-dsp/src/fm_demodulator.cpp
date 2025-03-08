#include "../include/lib-dsp/fm_demodulator.h"

#include "utils.h"

using namespace dsp;

namespace {

template <typename T>
class fm_demodulator : public processor<T>
{
    T _prev_input = 0;

public:
    fm_demodulator() {}

    // processor<T>
private:
    T process(T s) override { return s; }
    std::span<T> process(std::span<T> const& data) override
    {
        for (size_t i = 0; i < data.size(); ++i) {
            auto prev = data[i];
            data[i] = std::abs(data[i] - _prev_input);
            data[i] *= DSP_2PI;
            _prev_input = prev;
        }
        return data;
    }
};
} // namespace

namespace dsp {


template <>
std::unique_ptr<processor<float>> make_fm_demodulator()
{
    return std::make_unique<fm_demodulator<float>>();
}

} // namespace dsp