//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 28.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cmath>
#include <limits>
#include <type_traits>

namespace Yson
{
    template <typename T, typename U, typename std::enable_if<
            std::is_floating_point<T>::value == std::is_floating_point<U>::value
            && sizeof(T) >= sizeof(U), int>::type = 0>
    bool assignFloat(T& destination, U source)
    {
        destination = source;
        return true;
    }

    template <typename T, typename U, typename std::enable_if<
            std::is_floating_point<T>::value == std::is_floating_point<U>::value
            && sizeof(T) < sizeof(U), int>::type = 0>
    bool assignFloat(T& destination, U source)
    {
        if ((std::numeric_limits<T>::min() <= source
             && source <= std::numeric_limits<T>::max())
            || !std::isfinite(source))
        {
            destination = static_cast<T>(source);
            return true;
        }
        return false;
    }
}
