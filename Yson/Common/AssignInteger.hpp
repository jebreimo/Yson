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
#include "SelectTypeIf.hpp"

namespace Yson
{
    struct SameSignedness {};

    struct SignedUnsigned {};

    struct UnsignedSigned {};

    struct DestinationIsSmaller {};

    struct DestinationIsGreaterOrEqual {};

    struct DestinationIsSmallerOrEqual {};

    struct DestinationIsGreater {};

    template <typename T, typename U>
    bool assignIntegerImpl(T& destination, U source,
                           SameSignedness, DestinationIsGreaterOrEqual)
    {
        destination = source;
        return true;
    }

    template <typename T, typename U>
    bool assignIntegerImpl(T& destination, U source,
                           SameSignedness, DestinationIsSmaller)
    {
        if (std::numeric_limits<T>::min() <= source
            && source <= std::numeric_limits<T>::max())
        {
            destination = static_cast<T>(source);
            return true;
        }
        return false;
    }

    template <typename T, typename U>
    bool assignIntegerImpl(T& destination, U source,
                           SignedUnsigned, DestinationIsGreater)
    {
        destination = static_cast<T>(source);
        return true;
    }

    template <typename T, typename U>
    bool assignIntegerImpl(T& destination, U source,
                           SignedUnsigned, DestinationIsSmallerOrEqual)
    {
        if (source <= U(std::numeric_limits<T>::max()))
        {
            destination = static_cast<T>(source);
            return true;
        }
        return false;
    }

    template <typename T, typename U>
    bool assignIntegerImpl(T& destination, U source,
                           UnsignedSigned, DestinationIsGreater)
    {
        if (0 <= source)
        {
            destination = static_cast<T>(source);
            return true;
        }
        return false;
    }

    template <typename T, typename U>
    bool assignIntegerImpl(T& destination, U source,
                           UnsignedSigned, DestinationIsSmallerOrEqual)
    {
        if (0 <= source && source <= U(std::numeric_limits<T>::max()))
        {
            destination = static_cast<T>(source);
            return true;
        }
        return false;
    }

    template <typename T, typename U>
    bool assignInteger(T& destination, U source)
    {
        using DifferentSignedness = typename SelectTypeIf<
                std::is_signed<T>::value,
                SignedUnsigned,
                UnsignedSigned>::type;
        using Signedness = typename SelectTypeIf<
                std::is_signed<T>::value == std::is_signed<U>::value,
                SameSignedness,
                DifferentSignedness>::type;
        using SameSignednessSizeType = typename SelectTypeIf<
                sizeof(T) < sizeof(U),
                DestinationIsSmaller,
                DestinationIsGreaterOrEqual>::type;
        using DifferentSignednessSizeType = typename SelectTypeIf<
                sizeof(U) < sizeof(T),
                DestinationIsGreater,
                DestinationIsSmallerOrEqual>::type;
        using SizeType = typename SelectTypeIf<
                std::is_signed<T>::value == std::is_signed<U>::value,
                SameSignednessSizeType,
                DifferentSignednessSizeType>::type;

        return assignIntegerImpl(destination, source,
                                 Signedness(), SizeType());
    }
}
