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
    span process(span const& buff) override
    {
        _o.write(reinterpret_cast<char const*>(buff.data()), buff.size() * sizeof(float));
        return buff;
    }
};
} // namespace

std::unique_ptr<dsp::processor<float>> make_raw_writer(std::filesystem::path const& path)
{
    return std::make_unique<raw_file_writer>(path);
}