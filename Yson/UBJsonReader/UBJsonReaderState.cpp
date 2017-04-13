//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonReaderState.hpp"

namespace Yson
{
    #define CASE_TYPE(value) \
        case UBJsonReaderState::value: return #value

    std::string toString(UBJsonReaderState::State state)
    {
        switch(state)
        {
        CASE_TYPE(INITIAL_STATE);
        CASE_TYPE(AT_END_OF_FILE);
        CASE_TYPE(AT_START);
        CASE_TYPE(AT_KEY);
        CASE_TYPE(AT_VALUE);
        CASE_TYPE(AFTER_VALUE);
        CASE_TYPE(AT_END);
        }
        return "<unknown state: " + std::to_string(int(state)) + ">";
    }
}
