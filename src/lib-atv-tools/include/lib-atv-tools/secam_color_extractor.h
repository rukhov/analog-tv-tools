#pragma once

#include "color_extractor.h"

namespace atv {

class secam_color_extractor : public color_extractor
{
public:
    secam_color_extractor() {}
    // color_extractor
private:
    void process(std::span<float const> const& chroma,
                 std::span<uint32_t> const& tags,
                 std::span<YUV> const& out_buff) override
    {
        for (auto i = 0; i < chroma.size(); ++i) {
            out_buff[i].u = chroma[i];
            out_buff[i].v = chroma[i];
        }
    }
};
} // namespace atv