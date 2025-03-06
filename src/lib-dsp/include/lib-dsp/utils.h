#pragma once

namespace dsp {

constexpr double DSP_PI = 3.1415926535897932384626433832795;
constexpr double DSP_2PI = 2 * DSP_PI;

inline double usec2samples(double samp_rate, double us)
{
    return us * samp_rate / 1000000;
}

// Seconds to SecondSamples
inline double sec2samples(double samp_rate, double sec) { return sec * samp_rate; }

// Hertz to Radian
inline double hz2rad(double samp_rate, double hz) { return hz / (samp_rate * DSP_2PI); }

// Hertz to Hertz per sample
inline double hz2rel_frq(double samp_rate, double hz) { return hz / samp_rate; }

inline double relTimeSec(double samp_rate, double sec) { return 1 / (sec * samp_rate); }

template <typename T>
inline T normalize_phase(T const& p)
{
    T phase_;
    phase_ = std::modf(p, &phase_);
    while (phase_ < .0) {
        phase_ += 1.;
    }
    return phase_;
}
} // namespace dsp