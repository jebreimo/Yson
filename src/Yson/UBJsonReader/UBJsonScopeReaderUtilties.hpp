//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 12.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "UBJsonTokenizer.hpp"

namespace Yson
{
    bool isValueToken(UBJsonTokenType tokenType);

    bool readKey(UBJsonTokenizer& tokenizer);

    bool readStartOfOptimizedValue(UBJsonTokenizer& tokenizer,
                                   UBJsonTokenType tokenType);

    bool readStartOfValue(UBJsonTokenizer& tokenizer,
                          UBJsonTokenType endToken = UBJsonTokenType::UNKNOWN_TOKEN);

    void skipValue(UBJsonTokenizer& tokenizer);
}
