
#include <gtest/gtest.h>

#include <Iir.h>

TEST(Iir1, BandPass)
{

    Iir::Butterworth::BandPass<5> f;

    f.setup(1000, 250, 50);

    auto r = f.response(.25);

    EXPECT_NEAR(r.real(), 1.0, .000001);
    EXPECT_NEAR(r.imag(), 0.0, .000001);
}
