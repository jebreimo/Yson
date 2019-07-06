//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-09-17.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Yson/YsonException.hpp"

#define YSON_THROW(msg) \
        throw YsonException((msg), __FILE__, __LINE__, __FUNCTION__)

#define THROW_FILE_NOT_FOUND(msg) \
        throw FileNotFound((msg), __FILE__, __LINE__, __FUNCTION__)
