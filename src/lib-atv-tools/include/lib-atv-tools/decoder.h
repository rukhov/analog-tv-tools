#pragma once

#include "standard.h"
#include "video_buffer.h"


namespace atv {

class decoder
{
public:
    virtual ~decoder() = default;

    static standard make_params(standard_e);

    virtual void process(uint64_t length,
                         float const* input,
                         short* video_yout = nullptr,
                         short* video_uout = nullptr,
                         short* video_vout = nullptr,
                         float* luma_out = nullptr,
                         float* dbg1 = nullptr,
                         float* dbg2 = nullptr,
                         float* dbg3 = nullptr) = 0;

    static std::unique_ptr<decoder>
    make(standard const& params, uint64_t samp_rate, video_buffer::frame_cb frame_cb);
};

} // namespace atv