//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 30.12.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "JsonTokenizerUtilities.hpp"

#include <algorithm>
#include <cassert>

namespace Yson
{
    Result nextStringToken(std::string_view string, bool isEndOfFile,
                           char quotes);

    JsonTokenType determineCommentType(std::string_view string);

    Result nextCommentToken(std::string_view string, bool isEndOfFile);

    Result findEndOfNewline(std::string_view string, bool isEndOfFile);

    Result findEndOfValue(std::string_view string, bool isEndOfFile);

    Result findEndOfWhitespace(std::string_view string);

    Result nextToken(std::string_view string, bool isEndOfFile)
    {
        if (string.empty())
        {
            if (isEndOfFile)
                return Result(JsonTokenType::END_OF_FILE, string.end(), false);
            else
                return Result(JsonTokenType::INCOMPLETE_TOKEN, string.end(), true);
        }

        switch (string[0])
        {
        case ' ':
        case '\t':
            return findEndOfWhitespace(string);
        case '\r':
        case '\n':
            return findEndOfNewline(string, isEndOfFile);
        case '[':
            return Result(JsonTokenType::START_ARRAY, string.begin() + 1);
        case ']':
            return Result(JsonTokenType::END_ARRAY, string.begin() + 1);
        case '{':
            return Result(JsonTokenType::START_OBJECT, string.begin() + 1);
        case '}':
            return Result(JsonTokenType::END_OBJECT, string.begin() + 1);
        case ':':
            return Result(JsonTokenType::COLON, string.begin() + 1);
        case ',':
            return Result(JsonTokenType::COMMA, string.begin() + 1);
        case '"':
            return nextStringToken(string, isEndOfFile, '"');
        case '\'':
            return nextStringToken(string, isEndOfFile, '\'');
        case '/':
            return nextCommentToken(string, isEndOfFile);
        default:
            return findEndOfValue(string, isEndOfFile);
        }
    }

    Result nextStringToken(std::string_view string, bool isEndOfFile, char quotes)
    {
        assert(!string.empty());
        assert(string[0] == quotes);
        bool escape = false;
        auto tokenType = JsonTokenType::STRING;
        for (size_t i = 1, n = string.size(); i < n; ++i)
        {
            auto c = string[i];
            if (c < 0x20 && 0 < c)
            {
                if (!escape)
                {
                    if (c == '\n')
                        return {JsonTokenType::INVALID_TOKEN, &string[i]};
                    tokenType = JsonTokenType::INVALID_TOKEN;
                }
                else if (c == '\n')
                {
                    tokenType = JsonTokenType::INTERNAL_MULTILINE_STRING;
                }
                else if (c == '\r')
                {
                    if (i + 1 < n && string[i + 1] == '\n')
                        ++i;
                    tokenType = JsonTokenType::INTERNAL_MULTILINE_STRING;
                }
                else
                {
                    tokenType = JsonTokenType::INVALID_TOKEN;
                }
            }
            if (escape)
            {
                escape = false;
            }
            else if (c == quotes)
            {
                return {tokenType, string.data() + i + 1};
            }
            else if (c == '\\')
            {
                escape = true;
            }
        }
        if (isEndOfFile)
            return {JsonTokenType::INVALID_TOKEN, string.end()};
        else
            return {tokenType, string.end(), true};
    }

    Result findEndOfBlockComment(std::string_view string,
                                 bool isEndOfFile)
    {
        bool precededByStar = false;
        for (auto it = string.begin(); it != string.end(); ++it)
        {
            if (*it == '/' && precededByStar)
                return Result(JsonTokenType::BLOCK_COMMENT, ++it);
            precededByStar = *it == '*';
        }
        if (isEndOfFile)
            return Result(JsonTokenType::INVALID_TOKEN, string.end());
        return Result(JsonTokenType::BLOCK_COMMENT, string.end(), true);
    }

    Result findEndOfLineComment(std::string_view string,
                                bool isEndOfFile)
    {
        for (auto it = string.begin(); it != string.end(); ++it)
        {
            if (*it == '\r' || *it == '\n')
                return Result(JsonTokenType::COMMENT, it);
        }
        return Result(JsonTokenType::COMMENT, string.end(), !isEndOfFile);
    }

    Result nextCommentToken(std::string_view string, bool isEndOfFile)
    {
        assert(!string.empty());
        assert(string[0] == '/');

        auto tokenType = determineCommentType(string);
        if (tokenType == JsonTokenType::VALUE)
        {
            auto result = findEndOfValue(string, isEndOfFile);
            result.tokenType = JsonTokenType::INVALID_TOKEN;
            return result;
        }

        if (tokenType == JsonTokenType::INCOMPLETE_TOKEN)
        {
            if (!isEndOfFile)
                return Result(JsonTokenType::COMMENT, string.end(), true);
            else
                return Result(JsonTokenType::INVALID_TOKEN, string.end(), false);
        }

        if (tokenType == JsonTokenType::COMMENT)
            return findEndOfLineComment(string.substr(2), isEndOfFile);
        else
            return findEndOfBlockComment(string.substr(), isEndOfFile);
    }

    Result findEndOfNewline(std::string_view string, bool isEndOfFile)
    {
        assert(!string.empty() && (string[0] == '\n' || string[0] == '\r'));

        bool precededByCr = false;
        for (auto it = string.begin(); it != string.end(); ++it)
        {
            if (*it == '\n')
                return Result(JsonTokenType::NEWLINE, ++it);
            else if (precededByCr)
                return Result(JsonTokenType::NEWLINE, it);
            precededByCr = true;
        }
        return Result(JsonTokenType::NEWLINE, string.end(), !isEndOfFile);
    }

    Result findEndOfValue(std::string_view string, bool isEndOfFile)
    {
        for (auto it = string.begin(); it != string.end(); ++it)
        {
            switch (*it)
            {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
            case '"':
            case ',':
            case ':':
            case '[':
            case ']':
            case '{':
            case '}':
                return Result(JsonTokenType::VALUE, it);
            case '/':
            case '*':
                if (it != string.begin() && *(it - 1) == '/')
                    return Result(JsonTokenType::VALUE, it - 1);
            default:
                break;
            }
        }
        return Result(JsonTokenType::VALUE, string.end(), !isEndOfFile);
    }

    Result findEndOfWhitespace(std::string_view string)
    {
        for (auto it = string.begin(); it != string.end(); ++it)
        {
            if (*it != ' ' && *it != '\t')
                return Result(JsonTokenType::WHITESPACE, it);
        }
        return Result(JsonTokenType::WHITESPACE, string.end());
    }

    JsonTokenType determineCommentType(std::string_view string)
    {
        assert(!string.empty() && string[0] == '/');
        if (string.size() > 1)
        {
            if (string[1] == '*')
                return JsonTokenType::BLOCK_COMMENT;
            else if (string[1] == '/')
                return JsonTokenType::COMMENT;
            else
                return JsonTokenType::VALUE;
        }
        return JsonTokenType::INCOMPLETE_TOKEN;
    }

    std::pair<size_t, size_t> countLinesAndColumns(std::string_view string)
    {
        size_t lines = 0, cols = 0;
        for (auto it = string.begin(); it != string.end(); ++it)
        {
            if (*it == '\n')
            {
                ++lines;
                cols = 1;
            }
            else if (*it == '\r')
            {
                cols = 1;
            }
            else
            {
                ++cols;
            }
        }
        return std::make_pair(lines, cols);
    }

    void addLinesAndColumns(size_t& lineNumber, size_t& columnNumber,
                            std::pair<size_t, size_t> addend)
    {
        if (addend.first == 0)
        {
            columnNumber += addend.second;
        }
        else
        {
            lineNumber += addend.first;
            columnNumber = addend.second;
        }
    }

    std::pair<char*, char*> findLineContinuation(char* from, char* to)
    {
        while (true)
        {
            auto next = std::find(from, to, '\\');
            if (next == to || next + 1 == to)
                return {to, to};
            if (*(next + 1) == '\n')
                return {next, next + 2};
            if (*(next + 1) == '\r')
            {
                if (next + 2 != to && *(next + 2) == '\n')
                    return {next, next + 3};
                return {next, next + 2};
            }
            from = next + 2;
        }
    }
}
