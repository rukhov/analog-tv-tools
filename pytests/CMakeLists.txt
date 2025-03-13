find_program(PYTEST "pytest")

#message("=================== atv-tools-cli dir: " ${CMAKE_BINARY_DIR})

set(ASSETS_DIR \ ${CMAKE_BINARY_DIR} )
set(PROJECTS_DIR \ ${CMAKE_SOURCE_DIR} )

add_test (NAME Python_Tests
  COMMAND pytest 
    --verbosity=1
    --projectPath ${PROJECTS_DIR}
    --assetsPath ${ASSETS_DIR}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)