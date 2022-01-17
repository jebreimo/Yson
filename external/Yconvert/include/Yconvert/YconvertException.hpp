//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-08-03.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>

/** @file
  * @brief Defines the exception thrown by Yconvert functions.
  */

namespace Yconvert
{
    /**
     * @brief The exception class used throughout Yconvert.
     */
    class YconvertException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}
