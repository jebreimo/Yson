##****************************************************************************
## Copyright © 2015 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2015-09-24.
##
## This file is distributed under the Simplified BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 2.8)

#add_subdirectory(Ytest)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Ytest/CMakeLists.txt")
    add_subdirectory(Ytest)
    set(HAS_YTEST true PARENT_SCOPE)
endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Ytest/CMakeLists.txt")
