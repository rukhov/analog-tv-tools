/*
 * This file is part of the analot-tv-tools project.
 * Copyright (c) 2025 Roman Ukhov.
 *
 * https://github.com/rukhov/analog-tv-tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "processor.h"

namespace dsp {

template <typename T>
std::unique_ptr<processor<T, T>> make_direct(std::span<const T> const& taps);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_band_pass(double centerFrequencyNormalised,
                                                double bandWidthNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_band_pass_inv_chebyshev(
    double centerFrequencyNormalised, double bandWidthNormalised, double stopBandAtten);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_band_pass_chebyshev(
    double centerFrequencyNormalised, double bandWidthNormalised, double passBandRippe);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_band_stop(double centerFrequencyNormalised,
                                                double bandWidthNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_high_pass(double cutoffFrequencyNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>> make_low_pass(double cutoffFrequencyNormalised);

template <typename T, size_t order>
std::unique_ptr<processor<T, T>>
make_low_pass_inv_chebyshev(double cutoffFrequencyNormalised,
                            double stopBandAttenuationDb);


} // namespace dsp