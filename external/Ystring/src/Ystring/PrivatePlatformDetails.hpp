//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2015-07-28.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include <cstdint>
#include "Ystring/PlatformDetails.hpp"

namespace Ystring
{
    template <typename T>
    struct InternalCharType
    {
        typedef T Type;
    };

    #define YSTRING_DEFINE_INTERNAL_CHAR_TYPE(type, internalType) \
        template <> \
        struct InternalCharType<type> \
        { \
            typedef internalType Type; \
        }

    YSTRING_DEFINE_INTERNAL_CHAR_TYPE(int8_t, char);
    YSTRING_DEFINE_INTERNAL_CHAR_TYPE(uint8_t, char);
    YSTRING_DEFINE_INTERNAL_CHAR_TYPE(int16_t, char16_t);
    YSTRING_DEFINE_INTERNAL_CHAR_TYPE(int32_t, char32_t);

    #ifdef YSTRING_WCHAR_IS_2_BYTES
        YSTRING_DEFINE_INTERNAL_CHAR_TYPE(wchar_t, char16_t);
    #else
        YSTRING_DEFINE_INTERNAL_CHAR_TYPE(wchar_t, char32_t);
    #endif

    #ifdef YSTRING_CPP11_CHAR_TYPES_SUPPORTED
        YSTRING_DEFINE_INTERNAL_CHAR_TYPE(uint16_t, char16_t);
        YSTRING_DEFINE_INTERNAL_CHAR_TYPE(uint32_t, char32_t);
    #endif

    template <typename T>
    auto internal_char_type_cast(T* s)
            -> typename InternalCharType<T>::Type*
    {
        typedef typename InternalCharType<T>::Type Type;
        return reinterpret_cast<Type*>(s);
    }

    template <typename T>
    auto internal_char_type_cast(const T* s)
            -> const typename InternalCharType<T>::Type*
    {
        typedef typename InternalCharType<T>::Type Type;
        return reinterpret_cast<const Type*>(s);
    }
}
