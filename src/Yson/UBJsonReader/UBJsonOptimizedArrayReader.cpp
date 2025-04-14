//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 13.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonOptimizedArrayReader.hpp"
#include "ThrowUBJsonReaderException.hpp"
#include "UBJsonScopeReaderUtilties.hpp"

namespace Yson
{
    bool UBJsonOptimizedArrayReader::nextKey(UBJsonTokenizer& tokenizer,
                                             UBJsonReaderState& state)
    {
        UBJSON_READER_THROW("Cannot call this function inside arrays.",
                            tokenizer);
    }

    bool UBJsonOptimizedArrayReader::nextValue(UBJsonTokenizer& tokenizer,
                                               UBJsonReaderState& state)
    {
        if (state.state == ReaderState::AT_VALUE)
            skipValue(tokenizer);
        if (state.valueIndex < state.valueCount)
        {
            ++state.valueIndex;
            if (state.valueType != UBJsonTokenType::UNKNOWN_TOKEN)
            {
                if (tokenizer.next(state.valueType))
                {
                    state.state = ReaderState::AT_VALUE;
                    return true;
                }
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            }

            if (tokenizer.next())
            {
                if (isValueToken(tokenizer.tokenType()))
                {
                    state.state = ReaderState::AT_VALUE;
                    return true;
                }
                UBJSON_READER_UNEXPECTED_TOKEN(tokenizer);
            }

            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        }

        state.state = ReaderState::AT_END;
        return false;
    }

    bool UBJsonOptimizedArrayReader::nextDocument(UBJsonTokenizer& tokenizer,
                                                  UBJsonReaderState& state)
    {
        UBJSON_READER_THROW("Cannot call this function inside arrays.",
                            tokenizer);
    }

    char UBJsonOptimizedArrayReader::scopeType() const
    {
        return '[';
    }
}
