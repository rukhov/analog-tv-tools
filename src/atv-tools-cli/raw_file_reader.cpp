#include "raw_file_reader.h"

namespace {

class raw_file_reader : public dsp::processor<float>
{
    uint64_t _total_written = 0;
    std::ifstream _i;

public:
    raw_file_reader(std::filesystem::path const& path)
        : _i(path, std::ios::in | std::ios::binary)
    {
        std::clog << std::format("Opened file for read: {}", path.string());
    }


    ~raw_file_reader() {}
    // processor<float>
private:
    span process(span const& buff) override
    {
        _i.read(reinterpret_cast<char*>(buff.data()), buff.size() * sizeof(float));

        return { buff.data(), _i.gcount() / sizeof(float) };
    }
};
} // namespace

std::unique_ptr<dsp::processor<float>> make_raw_reader(std::filesystem::path const& path)
{
    return std::make_unique<raw_file_reader>(path);
}