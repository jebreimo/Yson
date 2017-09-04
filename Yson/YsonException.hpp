//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 20.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <stdexcept>
#include <string>

namespace Yson
{
    #ifndef _NOEXCEPT
        #define _NOEXCEPT
    #endif

    /** @brief The exception class used by all functions in the Yson library.
      */
    class YsonException : public std::logic_error
    {
    public:
        YsonException()
            : std::logic_error("Unspecified error.")
        {}

        explicit YsonException(const std::string& msg)
            : std::logic_error(msg)
        {}

        YsonException(const std::string& msg,
                      const std::string& debugFileName,
                      int debugLineNumber,
                      const std::string& debugFunctionName)
            : std::logic_error(msg)
        {
            if (!debugFunctionName.empty())
                m_DebugLocation += debugFunctionName + "() in ";
            m_DebugLocation += debugFileName + ":"
                               + std::to_string(debugLineNumber);
        }

        const std::string& debugLocation() const _NOEXCEPT
        {
            return m_DebugLocation;
        }

        const std::string& debugMessage() const
        {
            if (!m_DebugLocation.empty())
                return what() + std::string(" [") + m_DebugLocation + "]";
            else
                return what();
        }
    private:
        std::string m_DebugLocation;
    };

    #define YSON_THROW(msg) \
        throw YsonException((msg), __FILE__, __LINE__, __FUNCTION__)
}
