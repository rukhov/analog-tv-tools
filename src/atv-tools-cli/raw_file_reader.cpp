#include "raw_file_reader.h"

namespace {

class raw_file_reader : public dsp::processor<float>
{
    uint64_t _total_written = 0;
    std::ifstream _i;
    std::vector<float> _buffer;

public:
    raw_file_reader(std::filesystem::path const& path)
        : _i(path, std::ios::in | std::ios::binary)
    {
        std::clog << std::format("Opened file for read: {}", path.string());
    }


    ~raw_file_reader() {}
    // processor<float>
private:
    dsp::processor<float>::out_span_t
    process(dsp::processor<float>::in_span_t const& buff) override
    {
        if (_buffer.size() < buff.size())
            _buffer.resize(buff.size());

        _i.read(reinterpret_cast<char*>(_buffer.data()), buff.size() * sizeof(float));

        return { _buffer.data(), _i.gcount() / sizeof(float) };
    }
};
} // namespace

std::unique_ptr<dsp::processor<float>> make_raw_reader(std::filesystem::path const& path)
{
    return std::make_unique<raw_file_reader>(path);
}