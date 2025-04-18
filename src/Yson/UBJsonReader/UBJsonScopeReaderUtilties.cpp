//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 12.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "UBJsonScopeReaderUtilties.hpp"

#include <vector>
#include "ThrowUBJsonReaderException.hpp"
#include "UBJsonReaderState.hpp"

namespace Yson
{
    bool isValueToken(UBJsonTokenType tokenType)
    {
        switch (tokenType)
        {
        case UBJsonTokenType::NO_OP_TOKEN:
        case UBJsonTokenType::END_OBJECT_TOKEN:
        case UBJsonTokenType::END_ARRAY_TOKEN:
            return false;
        default:
            return true;
        }
    }

    bool readKey(UBJsonTokenizer& tokenizer)
    {
        if (!tokenizer.next(UBJsonTokenType::OBJECT_KEY_TOKEN))
            UBJSON_READER_UNEXPECTED_TOKEN(tokenizer);
        if (tokenizer.tokenType() == UBJsonTokenType::STRING_TOKEN)
            return true;
        if (tokenizer.tokenType() == UBJsonTokenType::END_OBJECT_TOKEN)
            return false;
        UBJSON_READER_UNEXPECTED_TOKEN(tokenizer);
    }

    bool readStartOfValue(UBJsonTokenizer& tokenizer,
                          UBJsonTokenType endToken)
    {
        if (!tokenizer.next())
            UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        if (isValueToken(tokenizer.tokenType()))
            return true;
        if (tokenizer.tokenType() == endToken)
            return false;
        UBJSON_READER_UNEXPECTED_TOKEN(tokenizer);
    }

    bool readStartOfOptimizedValue(UBJsonTokenizer& tokenizer,
                                   UBJsonTokenType tokenType)
    {
        if (tokenType != UBJsonTokenType::UNKNOWN_TOKEN)
            return tokenizer.next(tokenType);

        if (tokenizer.next())
        {
            if (isValueToken(tokenizer.tokenType()))
                return true;
            UBJSON_READER_UNEXPECTED_TOKEN(tokenizer);
        }

        return false;
    }

    void skipOptimizedObject(UBJsonTokenizer& tokenizer);

    void skipOptimizedArray(UBJsonTokenizer& tokenizer);

    bool isTrivialValue(UBJsonTokenType type)
    {
        switch (type)
        {
        case UBJsonTokenType::UNKNOWN_TOKEN:
        case UBJsonTokenType::START_OBJECT_TOKEN:
        case UBJsonTokenType::END_OBJECT_TOKEN:
        case UBJsonTokenType::START_ARRAY_TOKEN:
        case UBJsonTokenType::END_ARRAY_TOKEN:
            return false;
        default:
            return true;
        }
    }

    bool carriesValue(UBJsonTokenType type)
    {
        switch (type)
        {
        case UBJsonTokenType::NULL_TOKEN:
        case UBJsonTokenType::NO_OP_TOKEN:
        case UBJsonTokenType::TRUE_TOKEN:
        case UBJsonTokenType::FALSE_TOKEN:
            return false;
        default:
            return true;
        }
    }

    void skipKeys(UBJsonTokenizer& tokenizer)
    {
        auto count = tokenizer.contentSize();
        for (size_t i = 0; i < count; ++i)
        {
            if (!tokenizer.skip(UBJsonTokenType::STRING_TOKEN))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        }
    }

    void skipKeysAndTrivialValues(UBJsonTokenizer& tokenizer)
    {
        auto count = tokenizer.contentSize();
        auto contentType = tokenizer.contentType();
        for (size_t i = 0; i < count; ++i)
        {
            if (!tokenizer.skip(UBJsonTokenType::STRING_TOKEN)
                || !tokenizer.skip(contentType))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        }
    }

    void skipKeysAndComplexValues(UBJsonTokenizer& tokenizer) // NOLINT(*-no-recursion)
    {
        auto count = tokenizer.contentSize();
        auto contentType = tokenizer.contentType();
        for (size_t i = 0; i < count; ++i)
        {
            if (!tokenizer.skip(UBJsonTokenType::STRING_TOKEN)
                || !tokenizer.skip(contentType))
            {
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            }
            skipValue(tokenizer);
        }
    }

    void skipOptimizedObject(UBJsonTokenizer& tokenizer)
    {
        if (!carriesValue(tokenizer.contentType()))
            skipKeys(tokenizer);
        else if (isTrivialValue(tokenizer.contentType()))
            skipKeysAndTrivialValues(tokenizer);
        else
            skipKeysAndComplexValues(tokenizer);
    }

    void skipTrivialValues(UBJsonTokenizer& tokenizer)
    {
        auto count = tokenizer.contentSize();
        auto contentType = tokenizer.contentType();
        for (size_t i = 0; i < count; ++i)
        {
            if (!tokenizer.skip(contentType))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
        }
    }

    void skipComplexValues(UBJsonTokenizer& tokenizer)
    {
        auto count = tokenizer.contentSize();
        auto contentType = tokenizer.contentType();
        for (size_t i = 0; i < count; ++i)
        {
            if (!tokenizer.skip(contentType))
                UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer);
            skipValue(tokenizer);
        }
    }

    void skipOptimizedArray(UBJsonTokenizer& tokenizer)
    {
        if (!carriesValue(tokenizer.contentType()))
            return;

        if (isTrivialValue(tokenizer.contentType()))
            skipTrivialValues(tokenizer);
        else
            skipComplexValues(tokenizer);
    }

    void skipObject(UBJsonTokenizer& tokenizer)
    {
        while (tokenizer.skip(UBJsonTokenType::OBJECT_KEY_TOKEN))
        {
          if (tokenizer.tokenType() == UBJsonTokenType::END_OBJECT_TOKEN)
            return;
          tokenizer.skip();
          skipValue(tokenizer);
        }
    }

    void skipArray(UBJsonTokenizer& tokenizer)
    {
        while (tokenizer.skip())
        {
          if (tokenizer.tokenType() == UBJsonTokenType::END_ARRAY_TOKEN)
            return;
          skipValue(tokenizer);
        }
    }

    void skipValue(UBJsonTokenizer& tokenizer)
    {
        std::vector<UBJsonTokenType> tokens;
        switch (tokenizer.tokenType())
        {
        case UBJsonTokenType::START_OBJECT_TOKEN:
            skipObject(tokenizer);
            break;
        case UBJsonTokenType::START_ARRAY_TOKEN:
            skipArray(tokenizer);
            break;
        case UBJsonTokenType::START_OPTIMIZED_OBJECT_TOKEN:
            skipOptimizedObject(tokenizer);
            return;
        case UBJsonTokenType::START_OPTIMIZED_ARRAY_TOKEN:
            skipOptimizedArray(tokenizer);
            return;
        case UBJsonTokenType::NO_OP_TOKEN:
        case UBJsonTokenType::END_ARRAY_TOKEN:
        case UBJsonTokenType::END_OBJECT_TOKEN:
            UBJSON_READER_UNEXPECTED_TOKEN(tokenizer);
        default:
            break;
        }
    }
}
