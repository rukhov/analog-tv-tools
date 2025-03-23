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

#include <gtest/gtest.h>

#include <gst/app/app.h>
#include <gst/gst.h>
#include <chrono>
#include <iostream>
#include <thread>

// Buffer data (simulated video frames)
const int frame_size = 640 * 480 * 3; // Example: 640x480 RGB frames
uint8_t frame_data[frame_size];

// Function to simulate filling the buffer with video data
void fill_buffer(uint8_t* buffer, int size)
{
    static int counter = 0;
    for (int i = 0; i < size; i++) {
        buffer[i] = counter++ % 256; // Simulate some video data
    }
}

// Callback to feed data into appsrc
static void need_data(GstElement* appsrc, guint unused_size, gpointer user_data)
{
    static GstClockTime timestamp = 0;
    GstBuffer* buffer;
    GstFlowReturn ret;

    // Fill the buffer with video data
    fill_buffer(frame_data, frame_size);

    // Create a new GStreamer buffer and copy the data into it
    buffer = gst_buffer_new_allocate(nullptr, frame_size, nullptr);
    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_WRITE);
    memcpy(map.data, frame_data, frame_size);
    gst_buffer_unmap(buffer, &map);

    // Set the timestamp and duration for the buffer
    GST_BUFFER_PTS(buffer) = timestamp;
    GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 30); // 30 FPS
    timestamp += GST_BUFFER_DURATION(buffer);

    // Push the buffer into appsrc
    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
    gst_buffer_unref(buffer);

    if (ret != GST_FLOW_OK) {
        std::cerr << "Error pushing buffer to appsrc" << std::endl;
    }
}

TEST(gstreamer, First)
{
    char* argv[] = { "itest" };
    int argc = 0;
    // Initialize GStreamer
    gst_init(&argc, (char***)&(argv));

    // Create the pipeline
    GstElement* pipeline =
        gst_parse_launch("appsrc name=source ! videoconvert ! autovideosink", nullptr);

    EXPECT_NE(pipeline, nullptr);
}