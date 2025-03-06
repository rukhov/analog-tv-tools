#pragma once

#include <lib-atv-tools/color.h>
#include <lib-atv-tools/standard.h>
#include <lib-atv-tools/video_buffer.h>

class video_writer
{
public:
    virtual ~video_writer() = default;
    virtual void process_frame(std::span<atv::YUV> const&,
                               size_t rect_width,
                               size_t rect_height,
                               size_t total_width,
                               size_t total_height) = 0;

    static std::unique_ptr<video_writer> make(std::filesystem::path const& file_path,
                                              atv::standard const& standard);
};
