##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-05-31.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.5)
project(Yson_Ystring)

set(PRIVATE_HEADERS
    Conversion.hpp
    Encoding.hpp
    EncodingInfo.hpp
    Endianness.hpp
    PlatformDetails.hpp
    PrivatePlatformDetails.hpp
    YstringDefinitions.hpp
    YstringException.hpp
    Conversion/AbstractDecoder.hpp
    Conversion/AbstractEncoder.hpp
    Conversion/Convert.hpp
    Conversion/Converter.hpp
    Conversion/ErrorHandlingPolicy.hpp
    Conversion/Utf16Decoder.hpp
    Conversion/Utf16Encoder.hpp
    Conversion/Utf32Decoder.hpp
    Conversion/Utf32Encoder.hpp
    Conversion/Utf8Decoder.hpp
    Conversion/Utf8Encoder.hpp
    Encodings/DecoderResult.hpp
    Encodings/DecodeUtf16-impl.hpp
    Encodings/DecodeUtf16.hpp
    Encodings/DecodeUtf32-impl.hpp
    Encodings/DecodeUtf32.hpp
    Encodings/DecodeUtf8-impl.hpp
    Encodings/DecodeUtf8.hpp
    Encodings/EncodeUtf16-impl.hpp
    Encodings/EncodeUtf16.hpp
    Encodings/EncodeUtf32-impl.hpp
    Encodings/EncodeUtf32.hpp
    Encodings/EncodeUtf8-impl.hpp
    Encodings/EncodeUtf8.hpp
    Escape/DecoderAlgorithms.hpp
    Unicode/UnicodeChars.hpp
    Escape/Escape.hpp
    Utilities/Endian.hpp
    Utilities/Iterators.hpp
    Utilities/Union16.hpp
    Utilities/Union32.hpp
    )

set(SOURCE_FILES
    EncodingInfo.cpp
    YstringException.cpp
    Conversion/AbstractDecoder.cpp
    Conversion/AbstractEncoder.cpp
    Conversion/Converter.cpp
    Conversion/Utf8Decoder.cpp
    Conversion/Utf8Encoder.cpp
    Encodings/EncodeUtf8.cpp
    Escape/Escape.cpp
    )

add_library(
    ${PROJECT_NAME} OBJECT
    ${PRIVATE_HEADERS}
    ${SOURCE_FILES}
    )

target_compile_definitions(${PROJECT_NAME} PRIVATE Ystring=Yson_Ystring)
