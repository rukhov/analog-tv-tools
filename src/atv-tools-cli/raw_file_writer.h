#pragma once

#include <lib-dsp/processor.h>

std::unique_ptr<dsp::processor<float>> make_raw_writer(std::filesystem::path const& path);