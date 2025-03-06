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