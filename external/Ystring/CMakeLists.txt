##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-05-31.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.7)
project(Yson_Ystring)

set(PRIVATE_HEADERS
    include/Ystring/PlatformDetails.hpp
    include/Ystring/YstringDefinitions.hpp
    include/Ystring/YstringException.hpp
    include/Ystring/Escape/Escape.hpp
    src/Ystring/Encodings/DecoderResult.hpp
    src/Ystring/Encodings/DecodeUtf8.hpp
    src/Ystring/Encodings/EncodeUtf8.hpp
    src/Ystring/Escape/DecoderAlgorithms.hpp
    )

set(SOURCE_FILES
    src/Ystring/Encodings/EncodeUtf8.cpp
    src/Ystring/Escape/Escape.cpp
    )

add_library(Yson_Ystring OBJECT
    ${PRIVATE_HEADERS}
    ${SOURCE_FILES}
    )

target_compile_definitions(Yson_Ystring
    PRIVATE
        $<$<BOOL:${MSVC}>:_SCL_SECURE_NO_WARNINGS>
        Ystring=Yson_Ystring
    )

target_compile_options(Yson_Ystring
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-fPIC>
    )

target_include_directories(Yson_Ystring BEFORE
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

add_library(Yson::Ystring ALIAS Yson_Ystring)
