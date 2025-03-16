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

#include <opencv2/core.hpp>    // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp> // Video write

using namespace std;
using namespace cv;

TEST(opencv, DISABLED_First)
{
    EXPECT_TRUE(true);
    const string dest_file = "c:\\tmp\\test-video.avi";

    Size S = Size((int)1920, (int)1080);

    VideoWriter outputVideo; // Open the output
    outputVideo.open(dest_file, VideoWriter::fourcc('M', 'J', 'P', 'G'), 1, S, true);
    EXPECT_TRUE(outputVideo.isOpened());

    Mat R(S, CV_8UC3, Scalar(0, 0, 255));
    EXPECT_NO_THROW(outputVideo << R);
    Mat G(S, CV_8UC3, Scalar(0, 255, 0));
    EXPECT_NO_THROW(outputVideo << G);
    Mat B(S, CV_8UC3, Scalar(255, 0, 0));
    EXPECT_NO_THROW(outputVideo << B);

    Mat gradient(S, CV_8UC3, Scalar(0, 0, 0));

    for (auto x = 0; x < S.width; ++x) {
        uint8_t C = uint8_t(float(x) * 255 / S.width);
        for (auto y = 0; y < S.height; ++y) {
            gradient.at<Vec3b>({ x, y }) = Vec3b(C, 0, 0);
        }
    }

    EXPECT_NO_THROW(outputVideo << gradient);
}