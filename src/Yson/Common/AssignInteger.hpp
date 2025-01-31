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
                           UnsignedSigned, DestinationIsGreaterOrEqual)
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
                           UnsignedSigned, DestinationIsSmaller)
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
                std::is_signed_v<T>,
                SignedUnsigned,
                UnsignedSigned>::type;
        using Signedness = typename SelectTypeIf<
                std::is_signed_v<T> == std::is_signed_v<U>,
                SameSignedness,
                DifferentSignedness>::type;
        using SameSignednessSizeType = typename SelectTypeIf<
                sizeof(T) < sizeof(U),
                DestinationIsSmaller,
                DestinationIsGreaterOrEqual>::type;
        using SignedUnsignedSizeType = typename SelectTypeIf<
                sizeof(U) < sizeof(T),
                DestinationIsGreater,
                DestinationIsSmallerOrEqual>::type;
        using UnsignedSignedSizeType = typename SelectTypeIf<
                sizeof(U) <= sizeof(T),
                DestinationIsGreaterOrEqual,
                DestinationIsSmaller>::type;
        using DifferentSignednessSizeType = typename SelectTypeIf<
                std::is_signed_v<T> && std::is_unsigned_v<U>,
                SignedUnsignedSizeType,
                UnsignedSignedSizeType>::type;
        using SizeType = typename SelectTypeIf<
                std::is_signed_v<T> == std::is_signed_v<U>,
                SameSignednessSizeType,
                DifferentSignednessSizeType>::type;

        return assignIntegerImpl(destination, source,
                                 Signedness(), SizeType());
    }
}
