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

namespace dsp {

constexpr double DSP_PI = 3.1415926535897932384626433832795;
constexpr double DSP_2PI = 2 * DSP_PI;

inline double usec2samples(double samp_rate, double us)
{
    return us * samp_rate / 1000000.;
}

// Seconds to SecondSamples
inline double sec2samples(double samp_rate, double sec) { return sec * samp_rate; }

// Hertz to Radian
inline double hz2rad(double samp_rate, double hz) { return hz / (samp_rate * DSP_2PI); }

// Hertz to Hertz per sample
inline double hz2rel_frq(double samp_rate, double hz) { return hz / samp_rate; }

// Hertz per sample to Hertz
inline double rel2hz_frq(double samp_rate, double frq) { return frq * samp_rate; }

inline double relTimeSec(double samp_rate, double sec) { return 1 / (sec * samp_rate); }

template <typename T>
inline T normalize_phase(T const& p)
{
    T phase_ = std::modf(p, &phase_);
    while (phase_ < .0) {
        phase_ += 1.;
    }
    return phase_;
}
} // namespace dsp