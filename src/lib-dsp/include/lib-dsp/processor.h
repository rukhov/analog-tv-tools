#pragma once

namespace dsp {

template <typename InT, typename OutT = InT>
class processor
{
public:
    using in_span_t = std::span<InT>;
    using out_span_t = std::span<OutT>;
    using span = in_span_t;

    virtual ~processor() = default;
    virtual out_span_t process(in_span_t const&) = 0;
    virtual OutT process(InT s) { return 0; };
};

} // namespace dsp