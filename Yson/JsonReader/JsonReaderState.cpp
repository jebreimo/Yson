//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonReaderState.hpp"

namespace Yson
{
    #define CASE_TYPE(value) \
        case JsonReaderState::value: return #value

    std::string toString(JsonReaderState state)
    {
        switch (state)
        {
        CASE_TYPE(INITIAL_STATE);
        CASE_TYPE(AT_END_OF_FILE);
        CASE_TYPE(AT_START);
        CASE_TYPE(AT_KEY);
        CASE_TYPE(AT_VALUE);
        CASE_TYPE(AFTER_VALUE);
        CASE_TYPE(AT_END);
        CASE_TYPE(FATAL_ERROR);
        }
        return "<unknown state: " + std::to_string(int(state)) + ">";
    }
}
