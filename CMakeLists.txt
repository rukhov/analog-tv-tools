cmake_minimum_required(VERSION 3.26)

#set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(atv-tools VERSION 1.0.0 LANGUAGES CXX C)

option(BUILD_GNURADIO_BLOCKS "Build the gnuradio blocks" ON)

enable_testing()
include(FetchContent)

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

find_package(GTest REQUIRED)
find_package(SndFile REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS circular_buffer program_options)
find_package(OpenCV REQUIRED)
find_package(soxr)

set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS Off)

if(UNIX)
#    add_compile_options( $<$<CONFIG:Debug>:-g3 -O0> )
    if( CMAKE_BUILD_TYPE EQUAL "Debug" )
        add_compile_options( -g3 -O0 )
    endif()
elseif(WIN32)
    add_compile_definitions("UNICODE")
    add_compile_definitions("_UNICODE")
    add_compile_definitions("BOOST_USE_WINAPI_VERSION=0x0602")
    add_compile_definitions("_WIN32_WINNT=0x0602")
    add_compile_definitions("BOOST_PROCESS_VERSION=2")
endif()

string(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
string(REPLACE "/Ob1" "/Ob0" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

add_compile_options( -DSIGLIB_DATA_FLOAT=1 )

FetchContent_Declare(
  siglib
  GIT_REPOSITORY https://github.com/rukhov/siglib.git
  GIT_TAG        origin/main
)

FetchContent_MakeAvailable(siglib)

add_subdirectory(src)
