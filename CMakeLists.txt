##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-09-21.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 2.8)
project(Yson)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_definitions(-DYstring=Yson_Ystring)

if(WIN32)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS
                    -D_CRT_SECURE_NO_WARNINGS
                    -DYSON_EXPORTS
                    -DYSTRING_NO_AUTO_IMPORT)
endif(WIN32)

add_subdirectory(Ystring)

set(PUBLIC_HEADER_FILES
    Yson/DetailedValueType.hpp
    Yson/DetailedValueType-impl.hpp
    Yson/JsonReader.hpp
    Yson/JsonWriter.hpp
    Yson/Reader.hpp
    Yson/StructureParameters.hpp
    Yson/UBJsonReader.hpp
    Yson/UBJsonValueType.hpp
    Yson/UBJsonWriter.hpp
    Yson/ValueType.hpp
    Yson/Writer.hpp
    Yson/YsonDefinitions.hpp
    Yson/YsonException.hpp
    )

set(SOURCE_FILES
    Yson/DetailedValueType.hpp
    Yson/DetailedValueType-impl.hpp
    Yson/JsonReader.hpp
    Yson/JsonWriter.hpp
    Yson/Reader.hpp
    Yson/StructureParameters.hpp
    Yson/UBJsonReader.hpp
    Yson/UBJsonValueType.hpp
    Yson/UBJsonWriter.hpp
    Yson/ValueType.hpp
    Yson/Writer.hpp
    Yson/YsonDefinitions.hpp
    Yson/YsonException.hpp
    Yson/Common/AssignInteger.hpp
    Yson/Common/Base64.cpp
    Yson/Common/Base64.hpp
    Yson/Common/DetailedValueType.cpp
    Yson/Common/GetDetailedValueType.cpp
    Yson/Common/GetDetailedValueType.hpp
    Yson/Common/GetUnicodeFileName.cpp
    Yson/Common/GetUnicodeFileName.hpp
    Yson/Common/GetValueType.cpp
    Yson/Common/GetValueType.hpp
    Yson/Common/ParseFloatingPoint.cpp
    Yson/Common/ParseFloatingPoint.hpp
    Yson/Common/ParseInteger.cpp
    Yson/Common/ParseInteger.hpp
    Yson/Common/SelectTypeIf.hpp
    Yson/Common/StringView.hpp
    Yson/Common/UBJsonValueType.cpp
    Yson/Common/ValueType.cpp
    Yson/Common/ValueTypeUtilities.cpp
    Yson/Common/ValueTypeUtilities.hpp
    Yson/JsonReader/JsonArrayReader.cpp
    Yson/JsonReader/JsonArrayReader.hpp
    Yson/JsonReader/JsonDocumentReader.cpp
    Yson/JsonReader/JsonDocumentReader.hpp
    Yson/JsonReader/JsonObjectReader.cpp
    Yson/JsonReader/JsonObjectReader.hpp
    Yson/JsonReader/JsonReader.cpp
    Yson/JsonReader/JsonReaderException.hpp
    Yson/JsonReader/JsonReaderState.cpp
    Yson/JsonReader/JsonReaderState.hpp
    Yson/JsonReader/JsonScopeReader.hpp
    Yson/JsonReader/JsonScopeReaderUtilities.cpp
    Yson/JsonReader/JsonScopeReaderUtilities.hpp
    Yson/JsonReader/JsonTokenizer.cpp
    Yson/JsonReader/JsonTokenizer.hpp
    Yson/JsonReader/JsonTokenizerUtilities.hpp
    Yson/JsonReader/JsonTokenizerUtilities.cpp
    Yson/JsonReader/JsonTokenType.cpp
    Yson/JsonReader/JsonTokenType.hpp
    Yson/JsonReader/TextBufferReader.hpp
    Yson/JsonReader/TextBufferReader.cpp
    Yson/JsonReader/TextFileReader.hpp
    Yson/JsonReader/TextFileReader.cpp
    Yson/JsonReader/TextReader.hpp
    Yson/JsonReader/TextStreamReader.hpp
    Yson/JsonReader/TextStreamReader.cpp
    Yson/JsonWriter/JsonWriter.cpp
    Yson/JsonWriter/IsJavaScriptIdentifier.cpp
    Yson/JsonWriter/IsJavaScriptIdentifier.hpp
    Yson/UBJsonReader/BinaryBufferReader.cpp
    Yson/UBJsonReader/BinaryBufferReader.hpp
    Yson/UBJsonReader/BinaryFileReader.cpp
    Yson/UBJsonReader/BinaryFileReader.hpp
    Yson/UBJsonReader/BinaryMutableBufferReader.cpp
    Yson/UBJsonReader/BinaryMutableBufferReader.hpp
    Yson/UBJsonReader/BinaryReader.hpp
    Yson/UBJsonReader/BinaryStreamReader.cpp
    Yson/UBJsonReader/BinaryStreamReader.hpp
    Yson/UBJsonReader/FromBigEndian.hpp
    Yson/UBJsonReader/UBJsonArrayReader.cpp
    Yson/UBJsonReader/UBJsonArrayReader.hpp
    Yson/UBJsonReader/UBJsonDocumentReader.cpp
    Yson/UBJsonReader/UBJsonDocumentReader.hpp
    Yson/UBJsonReader/UBJsonObjectReader.cpp
    Yson/UBJsonReader/UBJsonObjectReader.hpp
    Yson/UBJsonReader/UBJsonOptimizedArrayReader.cpp
    Yson/UBJsonReader/UBJsonOptimizedArrayReader.hpp
    Yson/UBJsonReader/UBJsonOptimizedObjectReader.cpp
    Yson/UBJsonReader/UBJsonOptimizedObjectReader.hpp
    Yson/UBJsonReader/UBJsonReader.cpp
    Yson/UBJsonReader/UBJsonReaderException.hpp
    Yson/UBJsonReader/UBJsonReaderState.cpp
    Yson/UBJsonReader/UBJsonReaderState.hpp
    Yson/UBJsonReader/UBJsonReaderUtilities.cpp
    Yson/UBJsonReader/UBJsonReaderUtilities.hpp
    Yson/UBJsonReader/UBJsonScopeReaderUtilties.cpp
    Yson/UBJsonReader/UBJsonScopeReaderUtilties.hpp
    Yson/UBJsonReader/UBJsonScopeReader.hpp
    Yson/UBJsonReader/UBJsonTokenizer.cpp
    Yson/UBJsonReader/UBJsonTokenizer.hpp
    Yson/UBJsonReader/UBJsonTokenizerUtilities.cpp
    Yson/UBJsonReader/UBJsonTokenizerUtilities.hpp
    Yson/UBJsonReader/UBJsonTokenType.cpp
    Yson/UBJsonReader/UBJsonTokenType.hpp
    Yson/UBJsonWriter/AssignFloat.hpp
    Yson/UBJsonWriter/UBJsonValueTraits.hpp
    Yson/UBJsonWriter/UBJsonWriter.cpp
    Yson/UBJsonWriter/UBJsonWriterUtilities.cpp
    Yson/UBJsonWriter/UBJsonWriterUtilities.hpp
    )

add_library(
    ${PROJECT_NAME} SHARED
    ${SOURCE_FILES}
    $<TARGET_OBJECTS:Yson_Ystring>
    )

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        MACOSX_RPATH ON
        DEBUG_OUTPUT_NAME "${PROJECT_NAME}.debug")

add_subdirectory(Externals)

if (HAS_YTEST)
     set(CTEST_OUTPUT_ON_FAILURE 1)
     add_subdirectory(UnitTest)
endif(HAS_YTEST)

add_subdirectory(Miscellaneous)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${PUBLIC_HEADER_FILES}
    DESTINATION include/${PROJECT_NAME})
