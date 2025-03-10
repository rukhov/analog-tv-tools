#include "raw_file_writer.h"

namespace {

class raw_file_writer : public dsp::processor<float>
{
    uint64_t _total_written = 0;
    std::ofstream _o;

public:
    raw_file_writer(std::filesystem::path const& path)
        : _o(path, std::ios::out | std::ios::binary)
    {
        std::clog << std::format("Opened file for write: {}", path.string());
    }


    ~raw_file_writer() {}
    // processor<float>
private:
    dsp::processor<float>::out_span_t
    process(dsp::processor<float>::in_span_t const& buff) override
    {
        _o.write(reinterpret_cast<char const*>(buff.data()), buff.size() * sizeof(float));
        return {};
    }
};
} // namespace

std::unique_ptr<dsp::processor<float>> make_raw_writer(std::filesystem::path const& path)
{
    return std::make_unique<raw_file_writer>(path);
}