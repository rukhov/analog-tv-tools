#pragma once

#include "processor.h"

namespace dsp {

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_band_pass(double centerFrequencyNormalised,
                                                double bandWidthNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_band_stop(double centerFrequencyNormalised,
                                                double bandWidthNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_high_pass(double cutoffFrequencyNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_low_pass(double cutoffFrequencyNormalised);

} // namespace dsp