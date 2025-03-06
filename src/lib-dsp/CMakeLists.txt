set(TARGET_NAME lib-dsp)

set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS Off)

add_library(${TARGET_NAME} STATIC)

target_precompile_headers(${TARGET_NAME} PRIVATE src/stdafx.h)

file(GLOB Src
	src/*.cpp
	src/*.h
)

target_sources(${TARGET_NAME} PRIVATE 
    ${Src}
) 

set(LIB_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

message("----------------> LIB_DIR: " ${LIB_DIR})

if(UNIX)
	target_link_libraries(${TARGET_NAME} 
		PUBLIC siglib
		PUBLIC Boost::circular_buffer 
		PUBLIC ${LIB_DIR}/libsoxr.a
	)
	target_compile_options(${TARGET_NAME} 
		PRIVATE -fPIC
		)
elseif(WIN32)
	target_link_libraries(${TARGET_NAME} 
		PUBLIC siglib
		PUBLIC Boost::circular_buffer 
		PUBLIC ${LIB_DIR}/soxr.lib
	)
endif()


target_include_directories(${TARGET_NAME}
	INTERFACE include 
	PRIVATE include/lib-dsp 
)

target_compile_definitions(${TARGET_NAME}
    PRIVATE -DSIGLIB_STATIC_LIB=1
)
