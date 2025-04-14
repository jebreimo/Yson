# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-05-04.
#
# This file is distributed under the Zero-Clause BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.13)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v3.4.0"
)
FetchContent_GetProperties(catch)
if (NOT catch_POPULATED)
    FetchContent_Populate(catch)
    add_subdirectory(${catch_SOURCE_DIR} ${catch_BINARY_DIR})
endif ()

add_executable(YconvertTest
    test_Converter.cpp
    test_Endian.cpp
    test_Utf8Decoder.cpp
    test_Utf8Encoder.cpp
    test_Utf16Decoder.cpp
    test_Utf16Encoder.cpp
    test_Utf32Encoder.cpp
    test_Utf32Decoder.cpp test_Encoding.cpp test_Convert.cpp)

target_link_libraries(YconvertTest
    PRIVATE
        Yconvert::Yconvert
        Catch2::Catch2WithMain
    )

target_compile_options(YconvertTest
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    )

target_include_directories(YconvertTest
    PRIVATE
        ../../src
    )

add_test(NAME YconvertTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/YconvertTest)
