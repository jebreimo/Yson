//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 06.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <algorithm>
#include <cassert>

namespace Yson
{
    #ifdef IS_BIG_ENDIAN
    inline void fromBigEndian(size_t count, char* buffer)
    {}

    inline void fromBigEndian(size_t count, char* buffer, size_t unitSize)
    {}

    template <int N>
    void fromBigEndian(char* dst, const char* src)
    {
        memcpy(dst, src, N);
    }

    #else

    template <int N>
    void fromBigEndian(char* buffer)
    {
        std::reverse(buffer, buffer + N);
    }

    template <>
    inline void fromBigEndian<1>(char* /*buffer*/)
    {}

    template <>
    inline void fromBigEndian<2>(char* buffer)
    {
      std::swap(buffer[0], buffer[1]);
    }

    template <>
    inline void fromBigEndian<4>(char* buffer)
    {
      std::swap(buffer[0], buffer[3]);
      std::swap(buffer[1], buffer[2]);
    }

    template <>
    inline void fromBigEndian<8>(char* buffer)
    {
      std::swap(buffer[0], buffer[7]);
      std::swap(buffer[1], buffer[6]);
      std::swap(buffer[2], buffer[5]);
      std::swap(buffer[3], buffer[4]);
    }

    template <int N>
    void fromBigEndian(void* dst, const void* src)
    {
        for (int i = 0; i < N; ++i)
            dst[i] = src[N - i];
    }

    template <>
    inline void fromBigEndian<1>(void* dst, const void* src)
    {
        auto cdst = static_cast<char*>(dst);
        auto csrc = static_cast<const char*>(src);
        *cdst = *csrc;
    }

    template <>
    inline void fromBigEndian<2>(void* dst, const void* src)
    {
        auto cdst = static_cast<char*>(dst);
        auto csrc = static_cast<const char*>(src);
        cdst[0] = csrc[1];
        cdst[1] = csrc[0];
    }

    template <>
    inline void fromBigEndian<4>(void* dst, const void* src)
    {
        auto cdst = static_cast<char*>(dst);
        auto csrc = static_cast<const char*>(src);
        cdst[0] = csrc[3];
        cdst[1] = csrc[2];
        cdst[2] = csrc[1];
        cdst[3] = csrc[0];
    }

    template <>
    inline void fromBigEndian<8>(void* dst, const void* src)
    {
        auto cdst = static_cast<char*>(dst);
        auto csrc = static_cast<const char*>(src);
        cdst[0] = csrc[7];
        cdst[1] = csrc[6];
        cdst[2] = csrc[5];
        cdst[3] = csrc[4];
        cdst[4] = csrc[3];
        cdst[5] = csrc[2];
        cdst[6] = csrc[1];
        cdst[7] = csrc[0];
    }

    inline void fromBigEndian(size_t size, char* buffer)
    {
        switch (size)
        {
        case 1:
          break;
        case 2:
          fromBigEndian<2>(buffer);
          break;
        case 4:
          fromBigEndian<4>(buffer);
          break;
        case 8:
          fromBigEndian<8>(buffer);
          break;
        default:
          std::reverse(buffer, buffer + size);
          break;
        }
    }

    inline void fromBigEndian(size_t size, char* buffer, size_t unitSize)
    {
        assert(size % unitSize == 0);
        if (size == unitSize)
        {
            std::reverse(buffer, buffer + unitSize);
        }
        else
        {
            for (size_t i = unitSize; i <= size; i += unitSize)
                std::reverse(buffer + i - unitSize, buffer + i);
        }
    }
    #endif
}
