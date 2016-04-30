//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-08-04.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "YstringException.hpp"

#include <sstream>

namespace Ystring
{
    YstringException::YstringException()
        : std::logic_error("Unspecified error.")
    {}

    YstringException::YstringException(const std::string& msg)
        : std::logic_error(msg)
    {}

    YstringException::YstringException(const std::string& msg,
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

    const char* YstringException::what() const _NOEXCEPT
    {
        if (!m_Message.empty())
            return m_Message.c_str();
        return std::exception::what();
    }
}
