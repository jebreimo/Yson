##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-05-31.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.7)

project(YsonTest)

set(SOURCE_FILES
    main.cpp
    test_GetDetailedValueType.cpp
    test_GetValueType.cpp
    test_Base64.cpp
    test_GetValueType.cpp
    test_IsJavaScriptIdentifier.cpp
    test_JsonReader.cpp
    test_JsonTokenizer.cpp
    test_JsonWriter.cpp
    test_MakeReader.cpp
    test_ParseDouble.cpp
    test_UBJsonReader.cpp
    test_UBJsonTokenizer.cpp
    test_UBJsonWriter.cpp)

add_executable(YsonTest
               ${SOURCE_FILES})

target_link_libraries(YsonTest
    Yson::Yson
    Ytest::Ytest)
