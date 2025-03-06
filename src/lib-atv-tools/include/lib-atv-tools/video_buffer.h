#pragma once

#include "color.h"
#include "cvbs.h"
#include "standard.h"

namespace atv {

class video_buffer
{
    void on_frame_finished()
    {
        if (!_cb)
            return;

        _cb({ _buff.data(), _buff.size() }, _view_cx, _view_cy, _cx, _cy);
    }

public:
    using frame_cb = std::function<void(std::span<YUV> const&,
                                        size_t rect_width,
                                        size_t rect_height,
                                        size_t total_width,
                                        size_t total_height)>;

    video_buffer(standard const& standard, uint64_t samp_rate, frame_cb& cb)
        : _cx(2400),
          _cy(640),
          _buff(_cx * _cy, _fill_color),
          _cb(cb),
          _view_cx(samp_rate / standard.hsync_pulse_frequency_hz),
          _view_cy(standard.total_line_count)
    {
        return;

        auto frame = _buff.data();

        for (auto x = 0u; x < _cx; ++x) {

            auto Y = float(x) / _cx;

            for (auto y = 0u; y < _cy; ++y) {

                frame[y * _cx + x] = Rgb2Yuv(RGB01{ 0, Y, 0 });
            }
        }
    }

    YCbCr process(YUV const& yuv, uint32_t tag)
    {
        // Write to buffer
        {
            auto frame = _buff.data();

            if (tag & cvbs_tag::hsync) {
                for (; _y < _cy && _x < _cx; ++_x) {
                    frame[_x + _y * _cx] = _fill_color;
                }
                _x = 0;
                _y += 2;
            }

            if (tag & (cvbs_tag::vsync_odd | cvbs_tag::vsync_even)) {
                auto start_x = _x;
                for (; _y < _cy; ++_y) {
                    for (auto x = start_x; x < _cx; ++x) {
                        frame[x + _y * _cx] = _fill_color;
                    }
                    start_x = 0;
                }
                _y = 0;

                if (tag & cvbs_tag::vsync_even)
                    on_frame_finished();

                if (tag & cvbs_tag::vsync_odd)
                    ++_y;
            }

            if (_x < _cx && _y < _cy) {
                frame[_x + _y * _cx] = yuv;
            }

            ++_x;
        }

        YCbCr pixel;

        // Extract pixel from buffer
        {
            auto rgb = Yuv2Rgb(_buff[_out_cur]);
            pixel = rgb2YCbCr(rgb);
        }

        ++_out_cur;
        if (_out_cur == _buff.size())
            _out_cur = 0;

        return pixel;
    }

private:
    const YUV _fill_color = Rgb2Yuv(RGB01(0, 0, 0));
    const size_t _cx;
    const size_t _cy;
    std::vector<YUV> _buff;
    size_t _y = 0;
    size_t _x = 0;
    size_t _out_cur = 0;
    frame_cb _cb;
    const size_t _view_cx, _view_cy;
};

} // namespace atv