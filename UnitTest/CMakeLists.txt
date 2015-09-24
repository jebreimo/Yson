##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-05-31
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.2)

set(PARENT_PROJECT Ystring)
project(${PARENT_PROJECT}Test)

add_subdirectory(Ytest)

if(MSVC)
    set(SOURCE_FILES_PLATFORM_SPECIFIC
        Utf16/test_Utf16WString.cpp
        )
    if(NOT MSVC_VERSION LESS 1900)
        set(SOURCE_FILES_PLATFORM_SPECIFIC
            ${SOURCE_FILES_PLATFORM_SPECIFIC}
            Utf8/test_Utf8StringCpp11.cpp
            )
    endif(NOT MSVC_VERSION LESS 1900)
else(MSVC)
    set(SOURCE_FILES_PLATFORM_SPECIFIC
        Utf8/test_Utf8StringCpp11.cpp
        Utf16/test_Utf16String.cpp
)
endif(MSVC)

set(SOURCE_FILES
    main.cpp
    test_EncodingInfo.cpp
    Generic/test_Convert.cpp
    Generic/test_GenericString.cpp
    Streaming/test_StreamFill.cpp
    Streaming/test_StreamJoin.cpp
    Unicode/test_CaseConverter.cpp
    Unicode/test_CaseInsensitive.cpp
    Unicode/test_CharClass.cpp
    Unicode/test_CharMap.cpp
    Unicode/test_CharMappingTypes.cpp
    Utf8/test_DecodeUtf8.cpp
    Utf8/test_EncodeUtf8.cpp
    Utf8/test_Utf8String.cpp
    Utf16/test_DecodeUtf16.cpp
    Utf16/test_EncodeUtf16.cpp
    Utf32/test_Utf32String.cpp
    ${SOURCE_FILES_PLATFORM_SPECIFIC}
    )

add_executable(${PROJECT_NAME}
               ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
                      ${PARENT_PROJECT}
                      Ytest)

add_test(${PARENT_PROJECT} ${PROJECT_NAME})
