//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <stdexcept>
#include <string>
#include "../YsonException.hpp"

/** @file
  * @brief Defines macros and functions used when JsonReader
  *     throws exceptions.
  */

namespace Yson
{
    inline YsonException makeYsonException(const std::string& msg,
                                           const JsonTokenizer& tokenizer,
                                           const std::string& fileName,
                                           int lineNo,
                                           const std::string& function)
    {
        std::string str;
        if (!tokenizer.fileName().empty())
            str = "In " + tokenizer.fileName() + " at ";
        else
            str = "At ";
        str += "line " + std::to_string(tokenizer.lineNumber())
               + " and column " + std::to_string(tokenizer.columnNumber())
               + ": " + msg;
        return {str, fileName, lineNo, function};
    }

    #define JSON_READER_THROW(msg, tokenizer) \
        throw makeYsonException((msg), tokenizer, \
                                __FILE__, __LINE__, __FUNCTION__)

    #define JSON_READER_UNEXPECTED_TOKEN(tokenizer) \
        throw makeYsonException( \
                "Unexpected token: '" + tokenizer.tokenString() + "'.", \
                tokenizer, __FILE__, __LINE__, __FUNCTION__)

    #define JSON_READER_UNEXPECTED_END_OF_DOCUMENT(tokenizer) \
        throw makeYsonException("Unexpected end of document.", tokenizer, \
                                __FILE__, __LINE__, __FUNCTION__)
}
