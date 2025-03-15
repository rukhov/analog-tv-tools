#pragma once

#include <lib-dsp/processor.h>

enum class DaraType { F32, I16 };

std::unique_ptr<dsp::processor<float>> make_raw_reader(std::filesystem::path const& path,
                                                       DaraType type = DaraType::F32);