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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
elseif(WIN32)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS
                    -DYSON_EXPORTS
                    -DYSTRING_NO_AUTO_IMPORT)
endif(APPLE)

add_subdirectory(Ystring)

set(PUBLIC_HEADERS
    Yson/JsonReader.hpp
    Yson/JsonReaderException.hpp
    Yson/JsonTokenType.hpp
    Yson/JsonTokenizer.hpp
    Yson/JsonWriter.hpp
    Yson/LineNumberCounter.hpp
    Yson/ValueType.hpp
    Yson/YsonDefinitions.hpp
    )

set(PRIVATE_HEADERS
    Yson/Base64.hpp
    Yson/GetValueType.hpp
    Yson/IsJavaScriptIdentifier.hpp
    Yson/ParseDouble.hpp
    Yson/ParseInteger.hpp
    Yson/Span.hpp
    Yson/TextFileReader.hpp
    Yson/TextReader.hpp
    )

set(SOURCE_FILES
    Yson/Base64.cpp
    Yson/GetValueType.cpp
    Yson/IsJavaScriptIdentifier.cpp
    Yson/JsonReader.cpp
    Yson/JsonReaderException.cpp
    Yson/JsonTokenizer.cpp
    Yson/JsonWriter.cpp
    Yson/LineNumberCounter.cpp
    Yson/ParseDouble.cpp
    Yson/ParseInteger.cpp
    Yson/TextFileReader.cpp
    Yson/TextReader.cpp)

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
