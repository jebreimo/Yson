//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <iosfwd>
#include <string>

namespace Yson
{
    enum class ReaderState
    {
        INITIAL_STATE,
        AT_END_OF_FILE,
        AT_START,
        AT_KEY,
        AT_VALUE,
        AFTER_VALUE,
        AT_END
    };

    std::string toString(ReaderState state);

    std::ostream& operator<<(std::ostream& stream, ReaderState state);
}
