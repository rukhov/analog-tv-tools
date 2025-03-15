#include "video_writer.h"

#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp> // Video write

using namespace cv;

namespace {

class video_writer_impl : public video_writer
{
    VideoWriter _outputVideo; // Open the output
    Size _frame_size;

public:
    video_writer_impl(std::filesystem::path const& file_path,
                      atv::standard const& standard)
        : _frame_size(1280, 960) //_frame_size((int)standard.total_line_count * 4 / 3,
                                 //(int)standard.total_line_count)
    {
        _outputVideo.open(file_path.string().c_str(),
                          VideoWriter::fourcc('M', 'J', 'P', 'G'),
                          (standard.interlaced ? standard.vsync_pulse_frequency_hz / 2
                                               : standard.vsync_pulse_frequency_hz),
                          _frame_size,
                          true);

        if (!_outputVideo.isOpened())
            throw std::runtime_error(
                std::format("Cannot open output file <{}>", file_path.string()));
    }

private:
    void process_frame(std::span<atv::YUV> const& src,
                       size_t rect_width,
                       size_t rect_height,
                       size_t total_width,
                       size_t total_height) override
    {
        // if (width != _frame_size.width || height != _frame_size.height) throw
        // std::format("Frame size is not proper <{}, >", width, height);

        auto RGB2Vec3d = [](const atv::RGB01& _rgb) -> Vec3b {
            atv::RGB01 rgb = _rgb;

            Vec3b retVal;

            constexpr float mul = .9;
            constexpr float add = .05;

            rgb.r = rgb.r * mul + add;
            rgb.g = rgb.g * mul + add;
            rgb.b = rgb.b * mul + add;

            rgb.fix();

            retVal[0] = rgb.b * 255.;
            retVal[1] = rgb.g * 255.;
            retVal[2] = rgb.r * 255.;

            return retVal;
        };

        Mat frame(Size{ (int)rect_width, (int)rect_height }, CV_8UC3, Scalar(0, 0, 0));

        for (auto y = 0; y < rect_height; ++y) {
            for (auto x = 0; x < rect_width; ++x) {
                auto yuv = src[x + y * total_width];
                frame.at<Vec3b>({ x, y }) = RGB2Vec3d(atv::Yuv2Rgb(yuv));
            }
        }


        Mat dst;
        cv::resize(frame, dst, _frame_size, 0, 0, cv::INTER_LINEAR);

        _outputVideo << dst;
    }
};
} // namespace

std::unique_ptr<video_writer> video_writer::make(std::filesystem::path const& file_path,
                                                 atv::standard const& standard)
{
    return std::make_unique<video_writer_impl>(file_path, standard);
}