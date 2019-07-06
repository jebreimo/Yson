##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-09-21.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.7)
project(Yson VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(external/Ystring)

set(PUBLIC_HEADER_FILES
    include/Yson/DetailedValueType.hpp
    include/Yson/DetailedValueType-impl.hpp
    include/Yson/JsonReader.hpp
    include/Yson/JsonWriter.hpp
    include/Yson/Reader.hpp
    include/Yson/StructureParameters.hpp
    include/Yson/UBJsonReader.hpp
    include/Yson/UBJsonValueType.hpp
    include/Yson/UBJsonWriter.hpp
    include/Yson/ValueType.hpp
    include/Yson/Writer.hpp
    include/Yson/YsonDefinitions.hpp
    include/Yson/YsonException.hpp
    include/Yson/YsonReaderException.hpp
    )

set(SOURCE_FILES
    include/Yson/DetailedValueType.hpp
    include/Yson/DetailedValueType-impl.hpp
    include/Yson/JsonReader.hpp
    include/Yson/JsonWriter.hpp
    include/Yson/Reader.hpp
    include/Yson/StructureParameters.hpp
    include/Yson/UBJsonReader.hpp
    include/Yson/UBJsonValueType.hpp
    include/Yson/UBJsonWriter.hpp
    include/Yson/ValueType.hpp
    include/Yson/Writer.hpp
    include/Yson/YsonDefinitions.hpp
    include/Yson/YsonException.hpp
    include/Yson/YsonReaderException.hpp
    src/Yson/Common/AssignInteger.hpp
    src/Yson/Common/Base64.cpp
    src/Yson/Common/Base64.hpp
    src/Yson/Common/DefaultBufferSize.cpp
    src/Yson/Common/DefaultBufferSize.hpp
    src/Yson/Common/DetailedValueType.cpp
    src/Yson/Common/GetDetailedValueType.cpp
    src/Yson/Common/GetDetailedValueType.hpp
    src/Yson/Common/GetUnicodeFileName.cpp
    src/Yson/Common/GetUnicodeFileName.hpp
    src/Yson/Common/GetValueType.cpp
    src/Yson/Common/GetValueType.hpp
    src/Yson/Common/IsJavaScriptIdentifier.cpp
    src/Yson/Common/IsJavaScriptIdentifier.hpp
    src/Yson/Common/MakeReader.cpp
    src/Yson/Common/ParseFloatingPoint.cpp
    src/Yson/Common/ParseFloatingPoint.hpp
    src/Yson/Common/ParseInteger.cpp
    src/Yson/Common/ParseInteger.hpp
    src/Yson/Common/SelectTypeIf.hpp
    src/Yson/Common/StringView.hpp
    src/Yson/Common/ThrowYsonException.hpp
    src/Yson/Common/UBJsonValueType.cpp
    src/Yson/Common/ValueType.cpp
    src/Yson/Common/ValueTypeUtilities.cpp
    src/Yson/Common/ValueTypeUtilities.hpp
    src/Yson/JsonReader/JsonArrayReader.cpp
    src/Yson/JsonReader/JsonArrayReader.hpp
    src/Yson/JsonReader/JsonDocumentReader.cpp
    src/Yson/JsonReader/JsonDocumentReader.hpp
    src/Yson/JsonReader/JsonObjectReader.cpp
    src/Yson/JsonReader/JsonObjectReader.hpp
    src/Yson/JsonReader/JsonReader.cpp
    src/Yson/JsonReader/ThrowJsonReaderException.hpp
    src/Yson/JsonReader/JsonReaderState.cpp
    src/Yson/JsonReader/JsonReaderState.hpp
    src/Yson/JsonReader/JsonScopeReader.hpp
    src/Yson/JsonReader/JsonScopeReaderUtilities.cpp
    src/Yson/JsonReader/JsonScopeReaderUtilities.hpp
    src/Yson/JsonReader/JsonTokenizer.cpp
    src/Yson/JsonReader/JsonTokenizer.hpp
    src/Yson/JsonReader/JsonTokenizerUtilities.hpp
    src/Yson/JsonReader/JsonTokenizerUtilities.cpp
    src/Yson/JsonReader/JsonTokenType.cpp
    src/Yson/JsonReader/JsonTokenType.hpp
    src/Yson/JsonReader/TextBufferReader.hpp
    src/Yson/JsonReader/TextBufferReader.cpp
    src/Yson/JsonReader/TextFileReader.hpp
    src/Yson/JsonReader/TextFileReader.cpp
    src/Yson/JsonReader/TextReader.hpp
    src/Yson/JsonReader/TextStreamReader.hpp
    src/Yson/JsonReader/TextStreamReader.cpp
    src/Yson/JsonWriter/JsonWriter.cpp
    src/Yson/JsonWriter/JsonWriterUtilities.cpp
    src/Yson/JsonWriter/JsonWriterUtilities.hpp
    src/Yson/UBJsonReader/BinaryBufferReader.cpp
    src/Yson/UBJsonReader/BinaryBufferReader.hpp
    src/Yson/UBJsonReader/BinaryFileReader.cpp
    src/Yson/UBJsonReader/BinaryFileReader.hpp
    src/Yson/UBJsonReader/BinaryReader.hpp
    src/Yson/UBJsonReader/BinaryStreamReader.cpp
    src/Yson/UBJsonReader/BinaryStreamReader.hpp
    src/Yson/UBJsonReader/FromBigEndian.hpp
    src/Yson/UBJsonReader/UBJsonArrayReader.cpp
    src/Yson/UBJsonReader/UBJsonArrayReader.hpp
    src/Yson/UBJsonReader/UBJsonDocumentReader.cpp
    src/Yson/UBJsonReader/UBJsonDocumentReader.hpp
    src/Yson/UBJsonReader/UBJsonObjectReader.cpp
    src/Yson/UBJsonReader/UBJsonObjectReader.hpp
    src/Yson/UBJsonReader/UBJsonOptimizedArrayReader.cpp
    src/Yson/UBJsonReader/UBJsonOptimizedArrayReader.hpp
    src/Yson/UBJsonReader/UBJsonOptimizedObjectReader.cpp
    src/Yson/UBJsonReader/UBJsonOptimizedObjectReader.hpp
    src/Yson/UBJsonReader/UBJsonReader.cpp
    src/Yson/UBJsonReader/ThrowUBJsonReaderException.hpp
    src/Yson/UBJsonReader/UBJsonReaderState.cpp
    src/Yson/UBJsonReader/UBJsonReaderState.hpp
    src/Yson/UBJsonReader/UBJsonReaderUtilities.cpp
    src/Yson/UBJsonReader/UBJsonReaderUtilities.hpp
    src/Yson/UBJsonReader/UBJsonScopeReaderUtilties.cpp
    src/Yson/UBJsonReader/UBJsonScopeReaderUtilties.hpp
    src/Yson/UBJsonReader/UBJsonScopeReader.hpp
    src/Yson/UBJsonReader/UBJsonTokenizer.cpp
    src/Yson/UBJsonReader/UBJsonTokenizer.hpp
    src/Yson/UBJsonReader/UBJsonTokenizerUtilities.hpp
    src/Yson/UBJsonReader/UBJsonTokenType.cpp
    src/Yson/UBJsonReader/UBJsonTokenType.hpp
    src/Yson/UBJsonWriter/AssignFloat.hpp
    src/Yson/UBJsonWriter/UBJsonValueTraits.hpp
    src/Yson/UBJsonWriter/UBJsonWriter.cpp
    src/Yson/UBJsonWriter/UBJsonWriterUtilities.cpp
    src/Yson/UBJsonWriter/UBJsonWriterUtilities.hpp
    )

add_library(
    Yson STATIC
    ${SOURCE_FILES}
    $<TARGET_OBJECTS:Yson_Ystring>
    )

target_compile_definitions(Yson
    PRIVATE
        Ystring=Yson_Ystring)

target_include_directories(Yson BEFORE
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/Ystring/include
    )

if (WIN32)
    target_compile_definitions(Yson
        PRIVATE
            _SCL_SECURE_NO_WARNINGS
            _CRT_SECURE_NO_WARNINGS
            YSON_EXPORTS
            YSTRING_NO_AUTO_IMPORT
        )
endif (WIN32)

set_target_properties(Yson
    PROPERTIES
        MACOSX_RPATH ON
        DEBUG_OUTPUT_NAME "Yson.debug")

add_library(Yson::Yson ALIAS Yson)

if (EXISTS "${CMAKE_SOURCE_DIR}/external/Ytest/CMakeLists.txt")
    add_subdirectory(external/Ytest EXCLUDE_FROM_ALL)
    add_subdirectory(UnitTest)

    enable_testing()
    add_test(UnitTest
        COMMAND UnitTest)
endif ()

add_subdirectory(misc)

include(GNUInstallDirs)
install(TARGETS Yson
    EXPORT YsonExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${PUBLIC_HEADER_FILES}
    DESTINATION include/Yson)
