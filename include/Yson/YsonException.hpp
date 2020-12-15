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
        #if (defined(_MSC_VER)) && (_MSC_VER < 1910)
            #define _NOEXCEPT
        #else
            #define _NOEXCEPT noexcept
        #endif
    #endif

    /** @brief The exception class used by all functions in the Yson library.
      */
    class YsonException : public std::runtime_error
    {
    public:
        YsonException()
            : std::runtime_error("Unspecified error.")
        {}

        explicit YsonException(const std::string& msg)
            : std::runtime_error(msg)
        {}

        YsonException(const std::string& msg,
                      const std::string& debugFileName,
                      int debugLineNumber,
                      const std::string& debugFunctionName)
            : std::runtime_error(msg)
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

        std::string debugMessage() const
        {
            if (!m_DebugLocation.empty())
                return what() + std::string(" [") + m_DebugLocation + "]";
            else
                return what();
        }
    private:
        std::string m_DebugLocation;
    };

    class FileNotFound : public YsonException
    {
    public:
        explicit FileNotFound(const std::string& fileName)
            : YsonException("File not found: " + fileName),
              m_FileName(fileName)
        {}

        FileNotFound(const std::string& fileName,
                     const std::string& debugFileName,
                     int debugLineNumber,
                     const std::string& debugFunctionName)
                : YsonException("File not found: " + fileName,
                                debugFileName,
                                debugLineNumber,
                                debugFunctionName),
                  m_FileName(fileName)
        {}

        const std::string& fileName() const
        {
            return m_FileName;
        }
    private:
        std::string m_FileName;
    };
}
