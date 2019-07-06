//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.02.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "JsonTokenizer.hpp"

namespace Yson
{
    bool isValueToken(JsonTokenType tokenType);

    void readColon(JsonTokenizer& tokenizer);

    bool readComma(JsonTokenizer& tokenizer, JsonTokenType endToken);

    bool readKey(JsonTokenizer& tokenizer,
                 JsonTokenType endToken = JsonTokenType::INVALID_TOKEN);

    bool readStartOfValue(JsonTokenizer& tokenizer,
                          JsonTokenType endToken = JsonTokenType::INVALID_TOKEN);

    void skipValue(JsonTokenizer& tokenizer);
}
