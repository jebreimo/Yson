//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 03.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "../YsonException.hpp"
#include "UBJsonTokenizer.hpp"

namespace Yson
{
    inline YsonException makeYsonException(const std::string& msg,
                                           const UBJsonTokenizer& tokenizer,
                                           const std::string& fileName,
                                           int lineNo,
                                           const std::string& function)
    {
        std::stringstream ss;
        if (!tokenizer.fileName().empty())
            ss << tokenizer.fileName() << ",";
        else
            ss << "At";
        ss << " position " << tokenizer.position() << ": " << msg;
        return YsonException(ss.str(), fileName, lineNo, function);
    }

    #define UBJSON_READER_THROW(msg, tokenizer) \
        throw makeYsonException((msg), tokenizer, \
                                __FILE__, __LINE__, __FUNCTION__)

    #define UBJSON_READER_UNEXPECTED_TOKEN(tokenizer) \
        throw makeYsonException( \
                "Unexpected token: " + toString((tokenizer).tokenType()), \
                tokenizer, __FILE__, __LINE__, __FUNCTION__)

    #define UBJSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer) \
        throw makeYsonException("Unexpected end of document.", tokenizer, \
                                __FILE__, __LINE__, __FUNCTION__)
}
