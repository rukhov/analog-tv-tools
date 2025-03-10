include(FetchContent)

FetchContent_Declare(
  siglib
  GIT_REPOSITORY https://github.com/rukhov/siglib.git
  GIT_TAG        origin/main
  CMAKE_ARGS
    -DSIGLIB_DATA_FLOAT=1
)
FetchContent_MakeAvailable(siglib)

add_compile_options( -DSIGLIB_DATA_FLOAT=1 )
