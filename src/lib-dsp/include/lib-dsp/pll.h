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