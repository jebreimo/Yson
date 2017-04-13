//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-12-04.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <sstream>
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
        std::stringstream ss;
        if (!tokenizer.fileName().empty())
            ss << tokenizer.fileName() << ",";
        else
            ss << "On";
        ss << " line " << tokenizer.lineNumber() << ", column "
           << tokenizer.columnNumber() << ": " << msg;
        return YsonException(ss.str(), fileName, lineNo, function);
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
