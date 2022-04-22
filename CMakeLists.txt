##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-09-21.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.16)
project(Yson VERSION 1.4.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Set YSON_MASTER_PROJECT to ON if Yson is the root level project.
if (NOT DEFINED YSON_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(YSON_MASTER_PROJECT ON)
    else ()
        set(YSON_MASTER_PROJECT OFF)
    endif ()
endif ()

# Test option
option(YSON_BUILD_TEST "Build tests" ${YSON_MASTER_PROJECT})

# Install option
option(YSON_INSTALL "Generate the install target" ${YSON_MASTER_PROJECT})

include(GNUInstallDirs)

set(YCONVERT_ISO_CODE_PAGES OFF)
set(YCONVERT_DOS_CODE_PAGES OFF)
set(YCONVERT_WIN_CODE_PAGES OFF)
set(YCONVERT_MAC_CODE_PAGES OFF)

add_subdirectory(external/Yconvert)

configure_file(src/Yson/Common/YsonVersion.hpp.in YsonVersion.hpp @ONLY)

add_library(Yson
    include/Yson/DetailedValueType.hpp
    include/Yson/JsonItem.hpp
    include/Yson/JsonReader.hpp
    include/Yson/JsonValueItem.hpp
    include/Yson/JsonWriter.hpp
    include/Yson/ObjectItem.hpp
    include/Yson/Reader.hpp
    include/Yson/ReaderIterators.hpp
    include/Yson/ReaderState.hpp
    include/Yson/StructureParameters.hpp
    include/Yson/UBJsonReader.hpp
    include/Yson/UBJsonValueItem.hpp
    include/Yson/UBJsonValueType.hpp
    include/Yson/UBJsonWriter.hpp
    include/Yson/ValueItem.hpp
    include/Yson/ValueType.hpp
    include/Yson/Writer.hpp
    include/Yson/YsonDefinitions.hpp
    include/Yson/YsonException.hpp
    include/Yson/YsonReaderException.hpp
    src/Yson/Common/ArrayItem.cpp
    src/Yson/Common/AssignInteger.hpp
    src/Yson/Common/Base64.cpp
    src/Yson/Common/Base64.hpp
    src/Yson/Common/DefaultBufferSize.cpp
    src/Yson/Common/DefaultBufferSize.hpp
    src/Yson/Common/DetailedValueType.cpp
    src/Yson/Common/Escape.cpp
    src/Yson/Common/Escape.hpp
    src/Yson/Common/GetDetailedValueType.cpp
    src/Yson/Common/GetDetailedValueType.hpp
    src/Yson/Common/GetUnicodeFileName.cpp
    src/Yson/Common/GetUnicodeFileName.hpp
    src/Yson/Common/GetValueType.cpp
    src/Yson/Common/GetValueType.hpp
    src/Yson/Common/IsJavaScriptIdentifier.cpp
    src/Yson/Common/IsJavaScriptIdentifier.hpp
    src/Yson/Common/JsonItem.cpp
    src/Yson/Common/MakeReader.cpp
    src/Yson/Common/ObjectItem.cpp
    src/Yson/Common/ParseFloatingPoint.cpp
    src/Yson/Common/ParseFloatingPoint.hpp
    src/Yson/Common/ParseInteger.cpp
    src/Yson/Common/ParseInteger.hpp
    src/Yson/Common/ReaderIterators.cpp
    src/Yson/Common/ReaderState.cpp
    src/Yson/Common/SelectTypeIf.hpp
    src/Yson/Common/StringView.hpp
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
    src/Yson/JsonReader/JsonScopeReader.hpp
    src/Yson/JsonReader/JsonScopeReaderUtilities.cpp
    src/Yson/JsonReader/JsonScopeReaderUtilities.hpp
    src/Yson/JsonReader/JsonTokenizer.cpp
    src/Yson/JsonReader/JsonTokenizer.hpp
    src/Yson/JsonReader/JsonTokenizerUtilities.hpp
    src/Yson/JsonReader/JsonTokenizerUtilities.cpp
    src/Yson/JsonReader/JsonTokenType.cpp
    src/Yson/JsonReader/JsonTokenType.hpp
    src/Yson/JsonReader/JsonValueItem.cpp
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
    src/Yson/UBJsonReader/UBJsonValueItem.cpp
    src/Yson/UBJsonWriter/AssignFloat.hpp
    src/Yson/UBJsonWriter/UBJsonValueTraits.hpp
    src/Yson/UBJsonWriter/UBJsonWriter.cpp
    src/Yson/UBJsonWriter/UBJsonWriterUtilities.cpp
    src/Yson/UBJsonWriter/UBJsonWriterUtilities.hpp
    $<TARGET_OBJECTS:Yson_Yconvert>
    )

target_compile_definitions(Yson
    PRIVATE
        Yconvert=Yson_Yconvert
    )

target_include_directories(Yson BEFORE
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/Yconvert/include
    )

target_compile_definitions(Yson
    PRIVATE
        $<$<BOOL:${MSVC}>:_SCL_SECURE_NO_WARNINGS>
        $<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>
        $<$<CXX_COMPILER_ID:MSVC>:YSON_EXPORTS>
    )

set_target_properties(Yson
    PROPERTIES
        MACOSX_RPATH ON
        DEBUG_OUTPUT_NAME "Yson.debug"
    )

add_library(Yson::Yson ALIAS Yson)

if (UPDATE_SINGLE_SRC)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_jsonwriter_cpp.sh.in
        make_jsonwriter_cpp.sh)
    add_custom_target(UpdateSingleSrc ALL
        BYPRODUCTS
            ${CMAKE_CURRENT_SOURCE_DIR}/single_src/JsonWriterComplete.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/single_src/JsonWriterComplete.cpp
        COMMAND
            sh "${CMAKE_CURRENT_BINARY_DIR}/make_jsonwriter_cpp.sh"
        )
    add_dependencies(UpdateSingleSrc Yson)
endif ()

##
## Unit tests
##

if (YSON_BUILD_TEST)
    if (EXISTS "${CMAKE_SOURCE_DIR}/external/Ytest/CMakeLists.txt")
        add_subdirectory(external/Ytest EXCLUDE_FROM_ALL)
    endif ()
    enable_testing()
    add_subdirectory(tests/YsonTest)
endif()

##
## "Export" the current build tree and make it possible for other modules
## in the same build tree to locate it with find_package.
##

export(TARGETS Yson
    NAMESPACE Yson::
    FILE YsonConfig.cmake)

##
## Installation
##

if (YSON_INSTALL)
    install(TARGETS Yson
        EXPORT YsonConfig
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

    install(EXPORT YsonConfig
        FILE
            YsonConfig.cmake
        NAMESPACE
            Yson::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/Yson
        )

    install(
        DIRECTORY
            include/
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/YsonVersion.hpp
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}/Yson
        )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(YsonConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
        )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/YsonConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yson
        )
endif()

if (DEFINED LOCAL_SOURCE_PACKAGE_DIR OR DEFINED ENV{LOCAL_SOURCE_PACKAGE_DIR})
    if (NOT LOCAL_SOURCE_PACKAGE_DIR)
        set(LOCAL_SOURCE_PACKAGE_DIR $ENV{LOCAL_SOURCE_PACKAGE_DIR})
    endif ()
    if (YSON_MASTER_PROJECT)
        add_custom_target(${PROJECT_NAME}_BUILD_SRC_TARBALL
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND git archive -v --format=tar.gz --prefix=${PROJECT_NAME}/ HEAD > ${LOCAL_SOURCE_PACKAGE_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.tar.gz
            )
    endif ()
endif ()
