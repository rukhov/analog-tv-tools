#pragma once

#include "processor.h"

namespace dsp {

template <typename InT, typename OutT>
std::unique_ptr<processor<InT, OutT>> make_fm_demodulator();
} // namespace dsp