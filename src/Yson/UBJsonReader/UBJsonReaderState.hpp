//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Yson/DetailedValueType.hpp"
#include "UBJsonTokenType.hpp"

namespace Yson
{
    struct UBJsonReaderState
    {
        enum State
        {
            INITIAL_STATE,
            AT_END_OF_FILE,
            AT_START,
            AT_KEY,
            AT_VALUE,
            AFTER_VALUE,
            AT_END
        };

        UBJsonReaderState()
        {}

        UBJsonReaderState(State state)
                : state(state)
        {}

        UBJsonReaderState(State state,
                          size_t valueCount,
                          UBJsonTokenType valueType)
                : valueCount(valueCount),
                  valueType(valueType),
                  state(state)
        {}

        size_t valueCount = 0;
        size_t valueIndex = 0;
        UBJsonTokenType valueType = UBJsonTokenType::UNKNOWN_TOKEN;
        State state = INITIAL_STATE;
    };

    std::string toString(UBJsonReaderState::State state);
}
