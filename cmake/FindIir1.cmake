include(FetchContent)

FetchContent_Declare(
  iir1-lib
  GIT_REPOSITORY https://github.com/berndporr/iir1.git
  GIT_TAG        1.9.5
  CMAKE_ARGS
        -DIIR1_NO_EXCEPTIONS=OFF
        -DIIR1_INSTALL_STATIC=ON
        -DIIR1_BUILD_TESTING=OFF
        -DIIR1_BUILD_DEMO=OFF
)
FetchContent_MakeAvailable(iir1-lib)
