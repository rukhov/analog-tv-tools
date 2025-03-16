/*
 * This file is part of the analot-tv-tools project.
 * Copyright (c) 2025 Roman Ukhov.
 *
 * https://github.com/rukhov/analog-tv-tools
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "snd_file_reader.h"

namespace {

class snd_file_reader_impl : public dsp::processor<float>
{
    SNDFILE* _file = nullptr;
    uint64_t _total_read = 0;
    SF_INFO _info = {};
    std::vector<float> _buffer;

public:
    snd_file_reader_impl(std::filesystem::path const& path)
    {
        _file = sf_open(path.string().c_str(), SFM_READ, &_info);

        if (!_file)
            throw std::runtime_error(
                std::format("Cannot open sound file: {}", path.string()));

        if (_info.channels > 2)
            throw std::runtime_error(
                std::format("Unexpected channels number: {}", _info.channels));

        std::clog << std::format(
            "Opened file (fmt:{:x}): {}", _info.format, path.string());
    }


    ~snd_file_reader_impl() { sf_close(_file); }
    // processor<float>
private:
    dsp::processor<float>::out_span_t
    process(dsp::processor<float>::in_span_t const& buff) override
    {
        if (_buffer.size() < buff.size())
            _buffer.resize(buff.size());

        size_t readCount =
            sf_readf_float(_file, _buffer.data(), buff.size() / _info.channels);
        readCount *= _info.channels;
        _total_read += readCount;
        return { _buffer.data(), readCount };
    }
};
} // namespace

std::unique_ptr<dsp::processor<float>> make_snd_reader(std::filesystem::path const& path)
{
    return std::make_unique<snd_file_reader_impl>(path);
}