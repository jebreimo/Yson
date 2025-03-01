# ===========================================================================
# Copyright © 2025 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2025-02-28.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.17)

find_package(Doxygen)

if (Doxygen_NOT_FOUND)
    return()
endif ()

# Find all the public headers
file(GLOB_RECURSE YSON_DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/include/Yson/*.hpp")

list(APPEND YSON_DOXYGEN_INPUT "${CMAKE_CURRENT_BINARY_DIR}/../../YsonVersion.hpp")

# Use README.md as the main page
set(YSON_MARKDOWN_MAIN_PAGE "${CMAKE_SOURCE_DIR}/README.md")
list(INSERT YSON_DOXYGEN_INPUT 0 "${YSON_MARKDOWN_MAIN_PAGE}")

string(REPLACE ";" " \\\n    " DOXYGEN_INPUT "${YSON_DOXYGEN_INPUT}")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(DOXYFILE_IN Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

set(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
    DEPENDS ${YSON_DOXYGEN_INPUT}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
    MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
    COMMENT "Generating documentation with Doxygen")

# Nice named target so we can run the job easily
add_custom_target(YsonDoxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})
