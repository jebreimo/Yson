##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-09-21.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 2.8)
project(Yson)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_definitions(-DYstring=Yson_Ystring)

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
elseif(WIN32)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS
                    -DYSON_EXPORTS
                    -DYSTRING_NO_AUTO_IMPORT)
endif(APPLE)

add_subdirectory(Ystring)

set(PUBLIC_HEADERS
    Yson/JsonArray.hpp
    Yson/YsonException.hpp
    Yson/JsonInvalidValue.hpp
    Yson/JsonObject.hpp
    Yson/JsonSingleValue.hpp
    Yson/JsonValue.hpp
    Yson/LineNumberCounter.hpp
    Yson/Reader.hpp
    Yson/ReaderException.hpp
    Yson/TokenType.hpp
    Yson/Tokenizer.hpp
    Yson/ValueType.hpp
    Yson/Writer.hpp
    Yson/YsonDefinitions.hpp
    )

set(PRIVATE_HEADERS
    Yson/Detail/Base64.hpp
    Yson/Detail/GetValueType.hpp
    Yson/Detail/IsJavaScriptIdentifier.hpp
    Yson/Detail/ParseDouble.hpp
    Yson/Detail/ParseInteger.hpp
    Yson/Detail/Span.hpp
    Yson/Detail/TextBufferReader.hpp
    Yson/Detail/TextFileReader.hpp
    Yson/Detail/TextReader.hpp
    Yson/Detail/TextStreamReader.hpp
    )

set(SOURCE_FILES
    Yson/JsonArray.cpp
    Yson/JsonInvalidValue.cpp
    Yson/JsonObject.cpp
    Yson/JsonSingleValue.cpp
    Yson/JsonValue.cpp
    Yson/LineNumberCounter.cpp
    Yson/Reader.cpp
    Yson/Tokenizer.cpp
    Yson/Writer.cpp
    Yson/Detail/Base64.cpp
    Yson/Detail/GetValueType.cpp
    Yson/Detail/IsJavaScriptIdentifier.cpp
    Yson/Detail/ParseDouble.cpp
    Yson/Detail/ParseInteger.cpp
    Yson/Detail/TextBufferReader.cpp
    Yson/Detail/TextFileReader.cpp
    Yson/Detail/TextReader.cpp
    Yson/Detail/TextStreamReader.cpp
    )

add_subdirectory(Externals)

add_library(
    ${PROJECT_NAME} SHARED
    ${PUBLIC_HEADERS}
    ${PRIVATE_HEADERS}
    ${SOURCE_FILES}
    $<TARGET_OBJECTS:Yson_Ystring>
    )

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        MACOSX_RPATH ON
        DEBUG_OUTPUT_NAME "${PROJECT_NAME}.debug")

if (HAS_YTEST)
     set(CTEST_OUTPUT_ON_FAILURE 1)
     add_subdirectory(UnitTest)
endif(HAS_YTEST)

add_subdirectory(Miscellaneous)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${PUBLIC_HEADERS}
    DESTINATION include/${PROJECT_NAME})
