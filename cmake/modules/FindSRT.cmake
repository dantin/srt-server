# - Try to find libsrt
# Onece done this will define
#
# LIBSRT_FOUND - System has libsrt
# LIBSRT_INCLUDE_DIRS - The libsrt include directories
# LIBSRT_LIBRARIES - The libraries needed to use libsrt
# LIBSRT_DEFINITIONS - Compiler switches required for using libsrt

find_package(PkgConfig)
pkg_check_modules(PC_LIBSRT REQUIRED srt)
set(LIBSRT_DEFINITIONS ${PC_LIBSRT_CFLAGS_OTHER})

find_path(LIBSRT_INCLUDE_DIR srt/srt.h
          HINTS ${PC_LIBSRT_INCLUDEDIR} ${PC_LIBSRT_INCLUDE_DIRS}
          PATH_SUFFIXES srt)

find_library(LIBSRT_LIBRARY NAMES srt HINTS ${PC_LIBSRT_LIBDIR} ${PC_LIBSRT_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBSRT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(srt DEFAULT_MSG
                                  LIBSRT_LIBRARY LIBSRT_INCLUDE_DIR)

mark_as_advanced(LIBSRT_INCLUDE_DIR LIBSRT_LIBRARY)

set(LIBSRT_LIBRARIES ${LIBSRT_LIBRARY})
set(LIBSRT_INCLUDE_DIRS ${LIBSRT_INCLUDE_DIR})
