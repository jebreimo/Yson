//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 13.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonReaderUtilities.hpp"

namespace Yson
{
    UBJsonReaderState makeState(const UBJsonTokenizer& tokenizer,
                                unsigned options)
    {
        return {ReaderState::AT_START,
                tokenizer.contentSize(),
                tokenizer.contentType(),
                options};
    }
}
