#pragma once

namespace atv {

class color_extractor
{
public:
    virtual ~color_extractor() = default;
    virtual void process(std::span<const float> const& chroma,
                         std::span<uint32_t> const& tags,
                         std::span<YUV> const& out_buff) = 0;
};

} // namespace atv
