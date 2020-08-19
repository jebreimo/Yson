//****************************************************************************
// Copyright © 2017, Trimble Inc.
// Created 2017-07-31 by JEB
//****************************************************************************
#include "GetUnicodeFileName.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include "Ystring/Conversion.hpp"
#include "Ystring/YstringException.hpp"
#include "ThrowYsonException.hpp"
#endif

namespace Yson
{
#if defined(_WIN32) || defined(_WIN64)
    std::wstring getUnicodeFileName(const std::string& fileName)
    {
        try
        {
            return Ystring::Conversion::convert<std::wstring>(
                    fileName,
                    Ystring::Encoding::UTF_8,
                    Ystring::Encoding::UTF_16,
                    Ystring::Conversion::ErrorHandlingPolicy::THROW);
        }
        catch (Ystring::YstringException&)
        {
            YSON_THROW("File name must be encoded as UTF-8.");
        }
    }
#else
    const std::string& getUnicodeFileName(const std::string& fileName)
    {
        return fileName;
    }
#endif
}
