include(FetchContent)

FetchContent_Declare(
  siglib
  GIT_REPOSITORY https://github.com/rukhov/siglib.git
  GIT_TAG        origin/main
)
FetchContent_MakeAvailable(siglib)
