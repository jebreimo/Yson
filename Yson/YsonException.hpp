//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 20.10.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <sstream>
#include <stdexcept>

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

        YsonException(const std::string& msg)
            : std::logic_error(msg)
        {}

        YsonException(const std::string& msg,
                      const std::string& filename,
                      int lineno,
                      const std::string& funcname)
            : std::logic_error(msg)
        {
            std::stringstream ss;
            if (!funcname.empty())
                ss << funcname << "() in ";
            ss << filename << ":" << lineno << ": " << msg;
            m_Message = ss.str();
        }

        const char* what() const _NOEXCEPT
        {
            if (!m_Message.empty())
                return m_Message.c_str();
            return std::exception::what();
        }
    private:
        std::string m_Message;
    };

    #define YSON_THROW(msg) \
        throw YsonException((msg), __FILE__, __LINE__, __FUNCTION__)
}
