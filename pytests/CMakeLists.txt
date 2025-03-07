find_program(PYTEST "pytest")

#message("=================== atv-tools-cli dir: " ${CMAKE_BINARY_DIR})

set(ATV_TOOL_CLI \ $<TARGET_FILE:atv-tools-cli> )

add_test (NAME Python_Tests
  COMMAND pytest 
    --verbosity=1
    --decoderPath ${ATV_TOOL_CLI}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)