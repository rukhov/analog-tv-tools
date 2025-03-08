include(FetchContent)

FetchContent_Declare(
  liquid-dsp
  GIT_REPOSITORY https://github.com/jgaeddert/liquid-dsp.git
  GIT_TAG        v1.7.0
)
FetchContent_MakeAvailable(liquid-dsp)

add_library(liquid-dsp INTERFACE)
target_link_libraries(liquid-dsp INTERFACE liquid)
target_include_directories(liquid-dsp INTERFACE ${liquid-dsp_SOURCE_DIR}/include)
