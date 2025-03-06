find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_ATVTOOLS gnuradio-AtvTools)

FIND_PATH(
    GR_ATVTOOLS_INCLUDE_DIRS
    NAMES gnuradio/AtvTools/api.h
    HINTS $ENV{ATVTOOLS_DIR}/include
        ${PC_ATVTOOLS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_ATVTOOLS_LIBRARIES
    NAMES gnuradio-AtvTools
    HINTS $ENV{ATVTOOLS_DIR}/lib
        ${PC_ATVTOOLS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-AtvToolsTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_ATVTOOLS DEFAULT_MSG GR_ATVTOOLS_LIBRARIES GR_ATVTOOLS_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_ATVTOOLS_LIBRARIES GR_ATVTOOLS_INCLUDE_DIRS)
