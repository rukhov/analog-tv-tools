set(TARGET_NAME ${CMAKE_PROJECT_NAME}_integration_tests)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    main.cpp
    opencv_test.cpp
)

target_precompile_headers(${TARGET_NAME} PRIVATE stdafx.h)

set(LIB_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

target_link_libraries(${TARGET_NAME} PRIVATE GTest::gtest GTest::gtest_main opencv_videoio)

add_test(NAME "Integration tests" 
    COMMAND 
        $<TARGET_FILE:${TARGET_NAME}>
)
