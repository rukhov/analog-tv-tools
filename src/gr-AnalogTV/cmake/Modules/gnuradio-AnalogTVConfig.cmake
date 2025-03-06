find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_ANALOGTV gnuradio-AnalogTV)

FIND_PATH(
    GR_ANALOGTV_INCLUDE_DIRS
    NAMES gnuradio/AnalogTV/api.h
    HINTS $ENV{ANALOGTV_DIR}/include
        ${PC_ANALOGTV_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_ANALOGTV_LIBRARIES
    NAMES gnuradio-AnalogTV
    HINTS $ENV{ANALOGTV_DIR}/lib
        ${PC_ANALOGTV_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-AnalogTVTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_ANALOGTV DEFAULT_MSG GR_ANALOGTV_LIBRARIES GR_ANALOGTV_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_ANALOGTV_LIBRARIES GR_ANALOGTV_INCLUDE_DIRS)
