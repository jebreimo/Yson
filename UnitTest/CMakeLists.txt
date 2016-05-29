##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-05-31.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.2)

set(PARENT_PROJECT Yson)
project(${PARENT_PROJECT}Test)

set(SOURCE_FILES
    main.cpp
    test_GetValueType.cpp
    test_JsonReader.cpp
    test_JsonTokenizer.cpp
    test_JsonWriter.cpp
    test_ParseDouble.cpp
    test_Base64.cpp)

add_executable(${PROJECT_NAME}
               ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
                      ${PARENT_PROJECT}
                      Ytest)

add_test(${PARENT_PROJECT} ${PROJECT_NAME})
