#pragma once

#include "options.h"
#include <lib-dsp/processor.h>

std::unique_ptr<dsp::processor<float>> make_demodulator(options::in_data_type inputType,
                                                        double sample_rate,
                                                        double baseband_carrier_hz);