//****************************************************************************
// Copyright © 2020 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2020-05-06.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "Yconvert/YconvertException.hpp"

#define _YCONVERT_THROW_3(file, line, msg) \
    throw ::Yconvert::YconvertException(file ":" #line ": " msg)

#define _YCONVERT_THROW_2(file, line, msg) \
    _YCONVERT_THROW_3(file, line, msg)

#define YCONVERT_THROW(msg) \
    _YCONVERT_THROW_2(__FILE__, __LINE__, msg)
