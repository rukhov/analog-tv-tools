cmake_minimum_required(VERSION 3.26)

project(analog-tv-tools VERSION 1.0.0 LANGUAGES CXX C)

option(BUILD_GNURADIO_BLOCKS "Build the gnuradio blocks" ON)
option(BUILD_PYTHON_TESTS "Build python tests" ON)

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

enable_testing()

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

find_package(soxr)
find_package(GTest REQUIRED)
find_package(SndFile REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS circular_buffer program_options)
find_package(OpenCV REQUIRED)

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

include(cmake/FindSiglib.cmake)
include(cmake/FindIir1.cmake)
include(cmake/FindGStreamer.cmake)

add_subdirectory(src)
if(BUILD_PYTHON_TESTS)
    add_subdirectory(pytests)
endif()
