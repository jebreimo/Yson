##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-08-31.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
# Try to find Ystring
# Once done this will define
#  YSTRING_FOUND - System has Ystring
#  YSTRING_INCLUDE_DIRS - The Ystring include directories
#  YSTRING_LIBRARIES - The libraries needed to use Ystring
#  YSTRING_DEFINITIONS - Compiler switches required for using Ystring

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(LIBRARY_NAME Ystring.debug)
else(CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(LIBRARY_NAME Ystring)
endif(CMAKE_BUILD_TYPE MATCHES DEBUG)

if(WIN32)
    set(POSSIBLE_INCLUDE_PATHS
        $ENV{HOMEDRIVE}$ENV{HOMEPATH}/include
        )
    set(POSSIBLE_LIBRARY_PATHS
        $ENV{HOMEDRIVE}$ENV{HOMEPATH}/lib
        )
else(WIN32)
    set(POSSIBLE_INCLUDE_PATHS
        $ENV{HOME}/include
        )
    set(POSSIBLE_LIBRARY_PATHS
        $ENV{HOME}/lib
        )
    set(YSTRING_DEFINITIONS -std=c++11)
endif(WIN32)

find_path(YSTRING_INCLUDE_DIR Ystring/YstringDefinitions.hpp
          PATH_SUFFIXES Ystring
          PATHS ${POSSIBLE_INCLUDE_PATHS})

find_library(YSTRING_LIBRARY NAMES ${LIBRARY_NAME} lib${LIBRARY_NAME}
             PATHS ${POSSIBLE_LIBRARY_PATHS})

include(FindPackageHandleStandardArgs)

# Handle the QUIETLY and REQUIRED arguments and set YSTRING_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Ystring DEFAULT_MSG
                                  YSTRING_INCLUDE_DIR YSTRING_LIBRARY)

mark_as_advanced(YSTRING_INCLUDE_DIR YSTRING_LIBRARY)
