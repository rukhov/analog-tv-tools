#include <gtest/gtest.h>

#include "color.h"

namespace {

constexpr auto tolerance = .000001;

using namespace atv;

TEST(avt_tools, color)
{
    {
        RGB01 rgb = { 0.5, 0.5, 0.5 };

        auto yuv = Rgb2Yuv(rgb);
        rgb = Yuv2Rgb(yuv);

        EXPECT_NEAR(rgb.r, .5, tolerance);
        EXPECT_NEAR(rgb.g, .5, tolerance);
        EXPECT_NEAR(rgb.b, .5, tolerance);

        auto ydbdr = RGB2YDbDr(rgb);
        rgb = YDbDr2Rgb(ydbdr);

        EXPECT_NEAR(rgb.r, .5, tolerance);
        EXPECT_NEAR(rgb.g, .5, tolerance);
        EXPECT_NEAR(rgb.b, .5, tolerance);
    }
    {
        RGB01 rgb = { 1., 0., 1. };

        auto yuv = Rgb2Yuv(rgb);
        rgb = Yuv2Rgb(yuv);

        EXPECT_NEAR(rgb.r, 1., tolerance);
        EXPECT_NEAR(rgb.g, .0, tolerance);
        EXPECT_NEAR(rgb.b, 1., tolerance);

        auto ydbdr = RGB2YDbDr(rgb);
        rgb = YDbDr2Rgb(ydbdr);

        EXPECT_NEAR(rgb.r, 1., tolerance);
        EXPECT_NEAR(rgb.g, .0, tolerance);
        EXPECT_NEAR(rgb.b, 1., tolerance);
    }
    {
        RGB01 rgb = { 1., 1., .9 };

        auto yuv = Rgb2Yuv(rgb);
        rgb = Yuv2Rgb(yuv);

        EXPECT_NEAR(rgb.r, 1., tolerance);
        EXPECT_NEAR(rgb.g, 1., tolerance);
        EXPECT_NEAR(rgb.b, .9, tolerance);

        auto ydbdr = RGB2YDbDr(rgb);
        rgb = YDbDr2Rgb(ydbdr);

        EXPECT_NEAR(rgb.r, 1., tolerance);
        EXPECT_NEAR(rgb.g, 1., tolerance);
        EXPECT_NEAR(rgb.b, .9, tolerance);
    }
}

} // namespace
