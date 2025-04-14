//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 13.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonArrayReader.hpp"
#include "ThrowUBJsonReaderException.hpp"
#include "UBJsonScopeReaderUtilties.hpp"

namespace Yson
{
    bool UBJsonArrayReader::nextKey(UBJsonTokenizer& tokenizer,
                                    UBJsonReaderState& state)
    {
        UBJSON_READER_THROW("Cannot call this function inside arrays.",
                            tokenizer);
    }

    bool UBJsonArrayReader::nextValue(UBJsonTokenizer& tokenizer,
                                      UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            [[fallthrough]];
        case ReaderState::AT_START:
        case ReaderState::AFTER_VALUE:
            if (readStartOfValue(tokenizer, UBJsonTokenType::END_ARRAY_TOKEN))
            {
                state.state = ReaderState::AT_VALUE;
                return true;
            }
            state.state = ReaderState::AT_END;
            [[fallthrough]];
        default:
            return false;
        }
    }

    bool UBJsonArrayReader::nextDocument(UBJsonTokenizer& tokenizer,
                                         UBJsonReaderState& state)
    {
        UBJSON_READER_THROW(
                "Cannot call this function inside arrays.",
                tokenizer);
    }

    char UBJsonArrayReader::scopeType() const
    {
        return '[';
    }
}
