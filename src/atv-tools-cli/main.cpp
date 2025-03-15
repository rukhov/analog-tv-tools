#include <iostream>

#include <lib-atv-tools/decoder.h>
#include <lib-atv-tools/video_buffer.h>

#include <lib-dsp/level_corrector.h>
#include <lib-dsp/resampler.h>

#include "demodulator.h"
#include "options.h"
#include "raw_file_reader.h"
#include "raw_file_writer.h"
#include "snd_file_reader.h"
#include "timer.h"
#include "video_writer.h"


int main(int argc, char* argv[])
{
    timer::clock_t::duration decode_duration = {};
    timer::clock_t::duration video_encode_duration = {};

    try {
        std::cout << std::format(
            "Analogue TV decoding tools.\nCopyright (C) Roman Ukhov 2025.\n\n");

        options opts = read_options(argc, argv);

        auto const decoder_opts = atv::standard::make(opts.standard);

        opts.processing_sample_rate_hz = decoder_opts.chroma_subcarrier1_hz * 8;

        std::unique_ptr<dsp::processor<float>> reader;
        std::unique_ptr<dsp::processor<float>> demodulator;
        std::unique_ptr<video_writer> writer;

        if (opts.input_type == options::in_type::F32) {
            reader = make_raw_reader(opts.input_file, DaraType::F32);
        } else if (opts.input_type == options::in_type::I16) {
            reader = make_raw_reader(opts.input_file, DaraType::I16);
        } else {
            reader = make_snd_reader(opts.input_file);
        }

        if (opts.input_data_type == options::in_data_type::fm) {
            demodulator = make_demodulator(opts.input_data_type,
                                           opts.input_sample_rate_hz,
                                           opts.carrier_frequency_hz);
        }

        std::unique_ptr<dsp::processor<float>> raw_writer;
        if (opts.output_type == options::out_type::RAW) {
            raw_writer = make_raw_writer(opts.out_cvbs_raw_file);
        } else {
            writer = video_writer::make(opts.output_file, decoder_opts);
        }

        std::unique_ptr<dsp::resampler> resampler;

        if (0 && opts.processing_sample_rate_hz != opts.input_sample_rate_hz) {
            resampler = std::make_unique<dsp::resampler>(opts.input_sample_rate_hz,
                                                         opts.processing_sample_rate_hz);

            std::cout << std::format("Resampling to: {}\n",
                                     opts.processing_sample_rate_hz);
        } else {
            opts.processing_sample_rate_hz = opts.input_sample_rate_hz;
        }

        bool b_Stop = false;
        size_t frameNum = 0;
        auto frame_cb = [&](std::span<atv::YUV> const& frame,
                            size_t rect_width,
                            size_t rect_height,
                            size_t total_width,
                            size_t total_height) {
            std::cout << std::format("Frame: {}\r", frameNum) << std::flush;

            if (writer) {
                timer timer;
                writer->process_frame(
                    frame, rect_width, rect_height, total_width, total_height);
                video_encode_duration += timer.duration();
            }

            ++frameNum;

            if (opts.max_frame_number > 0 && frameNum == opts.max_frame_number)
                b_Stop = true;
        };

        std::vector<float> in_buffer(1024 * 4);
        dsp::level_corrector<float> level_corrector(opts.dc_correction,
                                                    opts.amplification);

        auto decoder =
            atv::decoder::make(decoder_opts, opts.processing_sample_rate_hz, frame_cb);

        std::cout << std::format("Start processing.\n");

        for (;;) {

            auto data = reader->process({ in_buffer.data(), in_buffer.size() });

            if (data.size() == 0)
                break;

            if (b_Stop == true)
                break;

            timer timer;

            if (demodulator) {
                data = demodulator->process(data);
            }

            level_corrector.process(data);

            if (resampler)
                data = resampler->process(data);

            if (raw_writer) {
                raw_writer->process(data);
                continue;
            }

            decoder->process(data.size(), data.data());

            decode_duration += timer.duration();
        }

        std::cout << std::format(
            "Done. ==================\nTotal frame number: {}\nProductivity: "
            "{:0.2f} FPS.\n",
            frameNum,
            frameNum > 0 ? (1. * 1000 /
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                (decode_duration - video_encode_duration) / frameNum)
                                .count())
                         : (0.));
    } catch (std::exception const& e) {
        std::cerr << std::format("Unhandled exception: {}\n", e.what());
    }

    return 0;
}