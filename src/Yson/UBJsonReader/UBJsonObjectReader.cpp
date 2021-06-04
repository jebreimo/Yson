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
        case ReaderState::AT_KEY:
            if (!tokenizer.next())
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            //[[fallthrough]]
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case ReaderState::AT_START:
        case ReaderState::AFTER_VALUE:
            if (readKey(tokenizer))
            {
                state.state = ReaderState::AT_KEY;
                return true;
            }
            else
            {
                state.state = ReaderState::AT_END;
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
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            //[[fallthrough]]
        case ReaderState::AT_START:
        case ReaderState::AFTER_VALUE:
            if (!readKey(tokenizer))
            {
                state.state = ReaderState::AT_END;
                return false;
            }
            //[[fallthrough]]
        case ReaderState::AT_KEY:
            if (readStartOfValue(tokenizer))
            {
                state.state = ReaderState::AT_VALUE;
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

    char UBJsonObjectReader::scopeType() const
    {
        return '{';
    }
}
