//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 15.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonOptimizedObjectReader.hpp"
#include "ThrowUBJsonReaderException.hpp"
#include "UBJsonScopeReaderUtilties.hpp"

namespace Yson
{
    bool UBJsonOptimizedObjectReader::nextKey(UBJsonTokenizer& tokenizer,
                                              UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case ReaderState::AT_KEY:
            if (!readStartOfOptimizedValue(tokenizer, state.valueType))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            // [[fallthrough]]
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            // [[fallthrough]]
        case ReaderState::AFTER_VALUE:
        case ReaderState::AT_START:
            if (state.valueIndex == state.valueCount)
            {
                state.state = ReaderState::AT_END;
                return false;
            }
            ++state.valueIndex;
            if (readKey(tokenizer))
            {
                state.state = ReaderState::AT_KEY;
                return true;
            }
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        default:
            return false;
        }
    }

    bool UBJsonOptimizedObjectReader::nextValue(UBJsonTokenizer& tokenizer,
                                                UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            // [[fallthrough]]
        case ReaderState::AFTER_VALUE:
        case ReaderState::AT_START:
            if (state.valueIndex == state.valueCount)
            {
                state.state = ReaderState::AT_END;
                return false;
            }
            ++state.valueIndex;
            if (!readKey(tokenizer))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            // [[fallthrough]]
        case ReaderState::AT_KEY:
            if (readStartOfOptimizedValue(tokenizer, state.valueType))
            {
                state.state = ReaderState::AT_VALUE;
                return true;
            }
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        default:
            return false;
        }
    }

    bool UBJsonOptimizedObjectReader::nextDocument(UBJsonTokenizer& tokenizer,
                                                   UBJsonReaderState& state)
    {
        UBJSON_READER_THROW("Cannot call this function inside arrays.",
                            tokenizer);
    }

    char UBJsonOptimizedObjectReader::scopeType() const
    {
        return '{';
    }
}
