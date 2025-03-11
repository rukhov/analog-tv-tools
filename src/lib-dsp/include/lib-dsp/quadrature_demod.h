#pragma once

#include "processor.h"

namespace dsp {

template <typename T, typename Tout = std::complex<T>>
std::unique_ptr<processor<T, Tout>> make_quadrature_demod(double currierFrequency);

} // namespace dsp