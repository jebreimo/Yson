//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 04.09.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>
#include <string>
#include "YsonException.hpp"

namespace Yson
{
    /** @brief The exception class used by functions in JsonReader/UBJsonReader.
      */
    class YsonReaderException : public YsonException
    {
    public:
        explicit YsonReaderException(const std::string& message,
                                     const std::string& debugLocation,
                                     std::string fileName = std::string(),
                                     size_t line = 0,
                                     size_t column = 0)
            : YsonException(makeLocationString(fileName, line, column)
                            + message, debugLocation),
              fileName(move(fileName)),
              line(line),
              column(column)
        {}

        std::string fileName;
        size_t line;
        size_t column;
    private:
        [[nodiscard]]
        static std::string makeLocationString(const std::string& fileName,
                                              size_t line,
                                              size_t column)
        {
            std::string result;
            if (!fileName.empty())
                result = "In " + fileName;
            if (line || column)
            {
                if (!result.empty())
                    result += " at";
                else
                    result = "At";
                if (line)
                    result += " line " + std::to_string(line);
                if (column)
                {
                    if (line)
                        result += " and";
                    result += " column " + std::to_string(column);
                }
            }
            if (!result.empty())
                result += ": ";
            return result;
        }
    };
}
