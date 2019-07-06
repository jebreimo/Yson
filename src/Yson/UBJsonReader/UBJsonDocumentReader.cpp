//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 26.02.2017.
//
// This file is distributed under the BSD License.
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
        case UBJsonReaderState::INITIAL_STATE:
            if (!readStartOfValue(tokenizer))
            {
                state.state = UBJsonReaderState::AT_END_OF_FILE;
                return false;
            }
        // [[fallthrough]]
        case UBJsonReaderState::AT_START:
            state.state = UBJsonReaderState::AT_VALUE;
            return true;
        case UBJsonReaderState::AFTER_VALUE:
        case UBJsonReaderState::AT_END:
        case UBJsonReaderState::AT_END_OF_FILE:
            return false;
        case UBJsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            if (!tokenizer.next())
            {
                state.state = UBJsonReaderState::AT_END_OF_FILE;
                return false;
            }
            state.state = UBJsonReaderState::AT_END;
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
        case UBJsonReaderState::INITIAL_STATE:
            if (!tokenizer.next())
            {
                state.state = UBJsonReaderState::AT_END_OF_FILE;
                return false;
            }
            state.state = UBJsonReaderState::AT_START;
            return true;
        case UBJsonReaderState::AT_END_OF_FILE:
            return false;
        case UBJsonReaderState::AT_START:
        case UBJsonReaderState::AT_VALUE:
            skipValue(tokenizer);
            // [[fallthrough]]
        case UBJsonReaderState::AFTER_VALUE:
            if (!tokenizer.next())
            {
                state.state = UBJsonReaderState::AT_END_OF_FILE;
                return false;
            }
            // [[fallthrough]]
        case UBJsonReaderState::AT_END:
            state.state = UBJsonReaderState::AT_START;
            return true;
        default:
            UBJSON_READER_THROW("Invalid state: " + toString(state.state)
                                + ".",
                                tokenizer);
        };
    }
}
