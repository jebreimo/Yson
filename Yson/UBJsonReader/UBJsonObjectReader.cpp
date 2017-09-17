//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 12.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonObjectReader.hpp"
#include "ThrowUBJsonReaderException.hpp"
#include "UBJsonScopeReaderUtilties.hpp"

namespace Yson
{
    bool UBJsonObjectReader::nextKey(UBJsonTokenizer& tokenizer,
                                     UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case UBJsonReaderState::AT_KEY:
            if (!tokenizer.next())
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            //[[fallthrough]]
        case UBJsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case UBJsonReaderState::AT_START:
        case UBJsonReaderState::AFTER_VALUE:
            if (readKey(tokenizer))
            {
                state.state = UBJsonReaderState::AT_KEY;
                return true;
            }
            else
            {
                state.state = UBJsonReaderState::AT_END;
                return false;
            }
        default:
            return false;
        }
    }

    bool UBJsonObjectReader::nextValue(UBJsonTokenizer& tokenizer,
                                       UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case UBJsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case UBJsonReaderState::AT_START:
        case UBJsonReaderState::AFTER_VALUE:
            if (!readKey(tokenizer))
            {
                state.state = UBJsonReaderState::AT_END;
                return false;
            }
            //[[fallthrough]]
        case UBJsonReaderState::AT_KEY:
            if (readStartOfValue(tokenizer))
            {
                state.state = UBJsonReaderState::AT_VALUE;
                return true;
            }
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        default:
            return false;
        }
    }

    bool UBJsonObjectReader::nextDocument(UBJsonTokenizer& tokenizer,
                                          UBJsonReaderState& state)
    {
        UBJSON_READER_THROW(
                "Cannot call this function inside objects.",
                tokenizer);
    }
}
