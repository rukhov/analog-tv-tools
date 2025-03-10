#pragma once

#include "processor.h"

namespace dsp {

template <typename T>
std::unique_ptr<processor<T, T>> make_fir_hilbert_transform(size_t length);

} // namespace dsp