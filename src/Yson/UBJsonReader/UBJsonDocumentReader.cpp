//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonDocumentReader.hpp"
#include "ThrowUBJsonReaderException.hpp"
#include "UBJsonScopeReaderUtilties.hpp"

namespace Yson
{

    bool UBJsonDocumentReader::nextKey(UBJsonTokenizer& tokenizer,
                                       UBJsonReaderState& state)
    {
        YSON_THROW("This function can only be called inside objects.");
    }

    bool UBJsonDocumentReader::nextValue(UBJsonTokenizer& tokenizer,
                                         UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case ReaderState::INITIAL_STATE:
            if (!readStartOfValue(tokenizer))
            {
                state.state = ReaderState::AT_END_OF_FILE;
                return false;
            }
        [[fallthrough]];
        case ReaderState::AT_START:
            state.state = ReaderState::AT_VALUE;
            return true;
        case ReaderState::AFTER_VALUE:
        case ReaderState::AT_END:
        case ReaderState::AT_END_OF_FILE:
            return false;
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            if (!tokenizer.next())
            {
                state.state = ReaderState::AT_END_OF_FILE;
                return false;
            }
            state.state = ReaderState::AT_END;
            return false;
        default:
            break;
        }
        return false;
    }

    bool UBJsonDocumentReader::nextDocument(UBJsonTokenizer& tokenizer,
                                            UBJsonReaderState& state)
    {
        switch (state.state)
        {
        case ReaderState::INITIAL_STATE:
            if (!tokenizer.next())
            {
                state.state = ReaderState::AT_END_OF_FILE;
                return false;
            }
            state.state = ReaderState::AT_START;
            return true;
        case ReaderState::AT_END_OF_FILE:
            return false;
        case ReaderState::AT_START:
        case ReaderState::AT_VALUE:
            skipValue(tokenizer);
            [[fallthrough]];
        case ReaderState::AFTER_VALUE:
            if (!tokenizer.next())
            {
                state.state = ReaderState::AT_END_OF_FILE;
                return false;
            }
            [[fallthrough]];
        case ReaderState::AT_END:
            state.state = ReaderState::AT_START;
            return true;
        default:
            UBJSON_READER_THROW("Invalid state: " + toString(state.state)
                                + ".",
                                tokenizer);
        }
    }

    char UBJsonDocumentReader::scopeType() const
    {
        return '\0';
    }
}
