#pragma once

#include "processor.h"

namespace dsp {

template <typename T>
std::unique_ptr<processor<T, T>> make_band_pass(size_t order,
                                                double centerFrequencyNormalised,
                                                double bandWidthNormalised);

} // namespace dsp