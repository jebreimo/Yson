//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 15.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonOptimizedObjectReader.hpp"
#include "UBJsonReaderException.hpp"
#include "UBJsonScopeReaderUtilties.hpp"

namespace Yson
{
    bool UBJsonOptimizedObjectReader::nextKey(UBJsonTokenizer& tokenizer,
                                              UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case UBJsonReaderState::AT_KEY:
            if (!readStartOfOptimizedValue(tokenizer, state.valueType))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            // [[fallthrough]]
        case UBJsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            // [[fallthrough]]
        case UBJsonReaderState::AFTER_VALUE:
        case UBJsonReaderState::AT_START:
            if (state.valueIndex == state.valueCount)
            {
                state.state = UBJsonReaderState::AT_END;
                return false;
            }
            ++state.valueIndex;
            if (readKey(tokenizer))
            {
                state.state = UBJsonReaderState::AT_KEY;
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
        case UBJsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            // [[fallthrough]]
        case UBJsonReaderState::AFTER_VALUE:
        case UBJsonReaderState::AT_START:
            if (state.valueIndex == state.valueCount)
            {
                state.state = UBJsonReaderState::AT_END;
                return false;
            }
            ++state.valueIndex;
            if (!readKey(tokenizer))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            // [[fallthrough]]
        case UBJsonReaderState::AT_KEY:
            if (readStartOfOptimizedValue(tokenizer, state.valueType))
            {
                state.state = UBJsonReaderState::AT_VALUE;
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
}
