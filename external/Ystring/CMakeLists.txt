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
    include/Ystring/Conversion.hpp
    include/Ystring/Encoding.hpp
    include/Ystring/EncodingInfo.hpp
    include/Ystring/Endianness.hpp
    include/Ystring/PlatformDetails.hpp
    include/Ystring/YstringDefinitions.hpp
    include/Ystring/YstringException.hpp
    include/Ystring/Conversion/Convert.hpp
    include/Ystring/Conversion/Converter.hpp
    include/Ystring/Conversion/ErrorHandlingPolicy.hpp
    include/Ystring/Escape/Escape.hpp
    include/Ystring/Unicode/UnicodeChars.hpp
    src/Ystring/PrivatePlatformDetails.hpp
    src/Ystring/Conversion/AbstractDecoder.hpp
    src/Ystring/Conversion/AbstractEncoder.hpp
    src/Ystring/Conversion/Utf16Decoder.hpp
    src/Ystring/Conversion/Utf16Encoder.hpp
    src/Ystring/Conversion/Utf32Decoder.hpp
    src/Ystring/Conversion/Utf32Encoder.hpp
    src/Ystring/Conversion/Utf8Decoder.hpp
    src/Ystring/Conversion/Utf8Encoder.hpp
    src/Ystring/Encodings/DecoderResult.hpp
    src/Ystring/Encodings/DecodeUtf16.hpp
    src/Ystring/Encodings/DecodeUtf32.hpp
    src/Ystring/Encodings/DecodeUtf8.hpp
    src/Ystring/Encodings/EncodeUtf16.hpp
    src/Ystring/Encodings/EncodeUtf32.hpp
    src/Ystring/Encodings/EncodeUtf8.hpp
    src/Ystring/Escape/DecoderAlgorithms.hpp
    src/Ystring/Utilities/Endian.hpp
    src/Ystring/Utilities/Iterators.hpp
    src/Ystring/Utilities/Union16.hpp
    src/Ystring/Utilities/Union32.hpp
    )

set(SOURCE_FILES
    src/Ystring/EncodingInfo.cpp
    src/Ystring/YstringException.cpp
    src/Ystring/Conversion/AbstractDecoder.cpp
    src/Ystring/Conversion/AbstractEncoder.cpp
    src/Ystring/Conversion/Converter.cpp
    src/Ystring/Conversion/Utf8Decoder.cpp
    src/Ystring/Conversion/Utf8Encoder.cpp
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

target_include_directories(Yson_Ystring BEFORE
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

add_library(Yson::Ystring ALIAS Yson_Ystring)
