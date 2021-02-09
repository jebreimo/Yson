//****************************************************************************
// Copyright © 2017, Trimble Inc.
// Created 2017-07-31 by JEB
//****************************************************************************
#include <Yconvert/YconvertException.hpp>
#include "GetUnicodeFileName.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include "Yconvert/Convert.hpp"
// #include "Ystring/YstringException.hpp"
#include "ThrowYsonException.hpp"
#endif

namespace Yson
{
#if defined(_WIN32) || defined(_WIN64)
    std::wstring getUnicodeFileName(const std::string& fileName)
    {
        try
        {
            return Yconvert::convertTo<std::wstring>(
                    fileName,
                    Yconvert::Encoding::UTF_8,
                    Yconvert::Encoding::UTF_16_NATIVE,
                    Yconvert::ErrorPolicy::THROW);
        }
        catch (Yconvert::YconvertException& ex)
        {
            YSON_THROW("File name must be encoded as UTF-8 (" + std::string(ex.what()) + ").");
        }
    }
#else
    const std::string& getUnicodeFileName(const std::string& fileName)
    {
        return fileName;
    }
#endif
}
