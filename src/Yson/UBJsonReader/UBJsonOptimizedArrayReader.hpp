//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 13.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "UBJsonScopeReader.hpp"

namespace Yson
{
    class UBJsonOptimizedArrayReader : public UBJsonScopeReader
    {
    public:
        bool nextKey(UBJsonTokenizer& tokenizer,
                     UBJsonReaderState& state) override;

        bool nextValue(UBJsonTokenizer& tokenizer,
                       UBJsonReaderState& state) override;

        bool nextDocument(UBJsonTokenizer& tokenizer,
                          UBJsonReaderState& state) override;

        [[nodiscard]] char scopeType() const override;
    };
}
