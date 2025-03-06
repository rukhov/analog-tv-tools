set(TARGET_NAME lib-atv-tools)

add_library(${TARGET_NAME} STATIC)

if(UNIX)
target_compile_options(${TARGET_NAME}
	PRIVATE -fPIC
)
endif()

target_precompile_headers(${TARGET_NAME} PRIVATE src/stdafx.h)

file(GLOB Src
	src/*.cpp
	src/*.h
)

if(WIN32)
    file(GLOB_RECURSE PlatformSrc
    	src/win/*.cpp
	)
endif(WIN32)

if(UNIX)
    file(GLOB_RECURSE PlatformSrc
    	src/linux/*.cpp
	)
endif(UNIX)

target_sources(${TARGET_NAME} PRIVATE 
    ${Src} ${PlatformSrc}
) 

#target_sources(myStaticLib PUBLIC FILE_SET HEADERS FILES myStaticLib.h)

target_link_libraries(${TARGET_NAME} PUBLIC lib-dsp Boost::circular_buffer)
target_include_directories(${TARGET_NAME} INTERFACE include PRIVATE include/lib-atv-tools)

#install(FILES cmake/Modules/gnuradio-AtvToolsConfig.cmake DESTINATION ${GR_CMAKE_DIR})

install(
	TARGETS 
		${TARGET_NAME}
)
