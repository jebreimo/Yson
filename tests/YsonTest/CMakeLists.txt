##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-05-31.
##
## This file is distributed under the Zero-Clause BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.16)

include(FetchContent)
FetchContent_Declare(ytest
    GIT_REPOSITORY "https://github.com/jebreimo/Ytest.git"
    GIT_TAG v0.1.6)
FetchContent_MakeAvailable(ytest)

add_executable(YsonTest
    main.cpp
    test_GetDetailedValueType.cpp
    test_GetValueType.cpp
    test_Base64.cpp
    test_GetValueType.cpp
    test_IsJavaScriptIdentifier.cpp
    test_JsonItem.cpp
    test_JsonReader.cpp
    test_JsonTokenizer.cpp
    test_JsonWriter.cpp
    test_MakeReader.cpp
    test_ParseDouble.cpp
    test_UBJsonReader.cpp
    test_UBJsonTokenizer.cpp
    test_UBJsonWriter.cpp
    test_ReaderIterators.cpp)

target_include_directories(YsonTest BEFORE
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

target_link_libraries(YsonTest
    Yson::Yson
    Ytest::Ytest
    )

add_test(NAME YsonTest COMMAND YsonTest)
