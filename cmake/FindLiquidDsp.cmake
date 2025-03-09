include(FetchContent)

FetchContent_Declare(
  liquid-dsp
  GIT_REPOSITORY https://github.com/rukhov/liquid-dsp.git
  GIT_TAG        windows-build
  CMAKE_ARGS
        -DBUILD_EXAMPLES=OFF
        -DBUILD_AUTOTESTS=OFF
        -DBUILD_BENCHMARKS=OFF
        -DENABLE_SIMD=OFF
        -DBUILD_SANDBOX=OFF
        -DBUILD_DOC=OFF
        -DCOVERAGE=OFF
)
FetchContent_MakeAvailable(liquid-dsp)

add_library(liquid-dsp INTERFACE)
target_link_libraries(liquid-dsp INTERFACE liquid)
target_include_directories(liquid-dsp INTERFACE ${liquid-dsp_SOURCE_DIR}/include)
