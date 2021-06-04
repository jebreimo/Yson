//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "JsonScopeReader.hpp"

namespace Yson
{
    class JsonDocumentReader : public JsonScopeReader
    {
    public:
        std::pair<ReaderState, bool> nextKey(
                JsonTokenizer& tokenizer,
                ReaderState state) override;

        std::pair<ReaderState, bool> nextValue(
                JsonTokenizer& tokenizer,
                ReaderState state) override;

        std::pair<ReaderState, bool> nextDocument(
                JsonTokenizer& tokenizer,
                ReaderState state) override;

        char scopeType() const override;
    };
}
