set(TARGET_NAME ${CMAKE_PROJECT_NAME}-cli)

set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS Off)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    main.cpp
    options.cpp
    snd_file_reader.cpp
    video_writer.cpp
    raw_file_writer.cpp
    raw_file_reader.cpp
    demodulator.cpp
)

target_precompile_headers(${TARGET_NAME} PRIVATE stdafx.h)

target_link_libraries(${TARGET_NAME} PRIVATE lib-dsp lib-atv-tools Boost::program_options SndFile::sndfile opencv_videoio)
