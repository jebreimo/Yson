//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2017-07-31.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <string>

namespace Yson
{
#if defined(_WIN32) || defined(_WIN64)
    std::wstring getUnicodeFileName(const std::string& fileName);
#else
    const std::string& getUnicodeFileName(const std::string& fileName);
#endif
}
