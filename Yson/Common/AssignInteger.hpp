//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 23.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <limits>
#include <type_traits>

namespace Yson
{
    template <typename T, typename U,
              typename std::enable_if<std::is_signed<T>::value == std::is_signed<U>::value
                                      && sizeof(T) >= sizeof(U), int>::type = 0>
    bool assignInteger(T& destination, U source)
    {
        destination = source;
        return true;
    }

    template <typename T, typename U,
              typename std::enable_if<std::is_signed<T>::value == std::is_signed<U>::value
                                      && sizeof(T) < sizeof(U), int>::type = 0>
    bool assignInteger(T& destination, U source)
    {
        if (std::numeric_limits<T>::min() <= source
            && source <= std::numeric_limits<T>::max())
        {
            destination = source;
            return true;
        }
        return false;
    }

    template <typename T, typename U,
              typename std::enable_if<std::is_signed<T>::value
                                      && std::is_unsigned<U>::value
                                      && sizeof(U) < sizeof(T), int>::type = 0>
    bool assignInteger(T& destination, U source)
    {
        destination = source;
        return true;
    }

    template <typename T, typename U,
              typename std::enable_if<std::is_signed<T>::value
                                      && std::is_unsigned<U>::value
                                      && sizeof(T) <= sizeof(U), int>::type = 0>
    bool assignInteger(T& destination, U source)
    {
        if (source <= U(std::numeric_limits<T>::max()))
        {
            destination = source;
            return true;
        }
        return false;
    }

    template <typename T, typename U,
              typename std::enable_if<std::is_unsigned<T>::value
                                      && std::is_signed<U>::value
                                      && sizeof(T) >= sizeof(U), int>::type = 0>
    bool assignInteger(T& destination, U source)
    {
        if (0 <= source)
        {
            destination = source;
            return true;
        }
        return false;
    }

    template <typename T, typename U,
            typename std::enable_if<std::is_unsigned<T>::value
                                    && std::is_signed<U>::value
                                    && sizeof(T) < sizeof(U), int>::type = 0>
    bool assignInteger(T& destination, U source)
    {
        if (0<= source && source <= U(std::numeric_limits<T>::max()))
        {
            destination = source;
            return true;
        }
        return false;
    }
}
