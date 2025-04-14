//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <string>
#include "Yson/DetailedValueType.hpp"
#include "Yson/ReaderState.hpp"
#include "UBJsonTokenType.hpp"

namespace Yson
{
    enum UBJsonReaderOptions
    {
        EXPAND_OPTIMIZED_BYTE_ARRAYS = 1
    };

    struct UBJsonReaderState
    {
        UBJsonReaderState() = default;

        UBJsonReaderState(ReaderState state,
                          unsigned options)
                : state(state),
                  options(options)
        {}

        UBJsonReaderState(ReaderState state,
                          size_t valueCount,
                          UBJsonTokenType valueType,
                          unsigned options)
                : valueCount(valueCount),
                  valueType(valueType),
                  state(state),
                  options(options)
        {}

        size_t valueCount = 0;
        size_t valueIndex = 0;
        UBJsonTokenType valueType = UBJsonTokenType::UNKNOWN_TOKEN;
        ReaderState state = ReaderState::INITIAL_STATE;
        unsigned options = EXPAND_OPTIMIZED_BYTE_ARRAYS;
    };
}
