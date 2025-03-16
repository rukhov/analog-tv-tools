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


#include <gtest/gtest.h>

#include <Iir.h>

TEST(Iir1, BandPass)
{
    constexpr auto tollerance = .001;

    Iir::Butterworth::BandPass<5> f;

    f.setup(1000, 250, 50);

    auto r = f.response(.25);
    EXPECT_NEAR(r.real(), 1.0, tollerance);
    EXPECT_NEAR(r.imag(), 0.0, tollerance);

    r = f.response(.1);
    EXPECT_NEAR(r.real(), 0.0, tollerance);
    EXPECT_NEAR(r.imag(), 0.0, tollerance);

    r = f.response(.4);
    EXPECT_NEAR(r.real(), 0.0, tollerance);
    EXPECT_NEAR(r.imag(), 0.0, tollerance);
}
