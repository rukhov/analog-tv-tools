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

#include "integrator.h"

namespace dsp {

template <typename T>
class contraphase_detector
{
    bool _state = false;
    dsp::integrator<float> _integrator;

public:
    contraphase_detector(double tau_samples) : _integrator(1 / tau_samples) {}

    bool state() const { return _state; }

    T process(T s1, T s2)
    {
        // inversion_phase_accum_ = (inversion_phase_accum_ * one_minus_ip_accum_coeff_) +
        // sample * ret_val * ip_accum_coeff_;

        if (s1 == 0 || s2 == 0)
            return _state;

        auto s = _integrator.process(s1 * s2);

        _state = s < 0 ? true : false;

        return _state;
    }
};
} // namespace dsp