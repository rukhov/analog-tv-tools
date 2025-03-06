#pragma once

#include <lib-dsp/processor.h>

std::unique_ptr<dsp::processor<float>> make_snd_reader(std::filesystem::path const& path);