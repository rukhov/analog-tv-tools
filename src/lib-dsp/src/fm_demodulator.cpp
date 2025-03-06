#include "../include/lib-dsp/fm_demodulator.h"

using namespace dsp;

namespace {

template <typename T>
class fm_demodulator : public processor<T>
{

public:
    fm_demodulator() {}

    // processor<T>
private:
    T process(T s) override { return s; }
    std::span<T> process(std::span<T> const& data) override { return data; }
};
} // namespace

namespace dsp {


template <>
std::unique_ptr<processor<float>> make_fm_demodulator()
{
    return std::make_unique<fm_demodulator<float>>();
}

} // namespace dsp