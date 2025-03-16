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