
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
