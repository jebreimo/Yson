//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "UBJsonReaderState.hpp"
#include "UBJsonTokenizer.hpp"

namespace Yson
{
    class UBJsonScopeReader
    {
    public:
        [[nodiscard]] virtual char scopeType() const = 0;

        virtual bool nextKey(UBJsonTokenizer& tokenizer,
                             UBJsonReaderState& state) = 0;

        virtual bool nextValue(UBJsonTokenizer& tokenizer,
                               UBJsonReaderState& state) = 0;

        virtual bool nextDocument(UBJsonTokenizer& tokenizer,
                                  UBJsonReaderState& state) = 0;
    };
}
