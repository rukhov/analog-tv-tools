#pragma once

namespace dsp {

class pll
{
public:
    virtual ~pll() = default;
    pll() = default;

    virtual double phase() const = 0;
    virtual void set_phase(double p) = 0;
    virtual float process(float s) = 0;
    virtual float state() const = 0;

    virtual std::span<float> process(std::span<float const> const&) = 0;

    static std::unique_ptr<pll> make_regular(double carrier_frequency,
                                             double cut_off_frequency);
    static std::unique_ptr<pll> make_costas(double carrier_frequency,
                                            double cut_off_frequency);
    static std::unique_ptr<pll> make_normalized(double carrier_frequency,
                                                double cut_off_frequency);
};
} // namespace dsp