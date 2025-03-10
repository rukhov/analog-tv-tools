#pragma once

#include "processor.h"

namespace dsp {

template <typename T>
std::unique_ptr<processor<T, std::complex<T>>>
make_quadrature_demod(double currierFrequency);

} // namespace dsp