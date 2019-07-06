//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-07-01.
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <cassert>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace Ystring { namespace Utilities
{
    template <typename FwdIt>
    bool advance(FwdIt& first, FwdIt last, size_t n,
                 std::forward_iterator_tag)
    {
        for (; n != 0; n--)
        {
            if (first == last)
                return false;
            ++first;
        }
        return true;
    }

    template <typename RndIt>
    bool advance(RndIt& first, RndIt last, size_t n,
                 std::random_access_iterator_tag)
    {
        auto len = std::distance(first, last);
        assert(len >= 0);
        if ((size_t)len < n)
            return false;
        first += n;
        return true;
    }

    template <typename It>
    bool advance(It& first, It last, size_t n = 1)
    {
        return advance(first, last, n,
                typename std::iterator_traits<It>::iterator_category());
    }

    template <typename FwdIt>
    bool recede(FwdIt first, FwdIt& last, size_t n, std::forward_iterator_tag)
    {
        FwdIt it = first;
        for (; n != 0; n--)
        {
            if (first == last)
                return false;
            ++first;
        }
        for (; first != last; ++first)
            ++it;
        last = it;
        return true;
    }

    template <typename BiIt>
    bool recede(BiIt first, BiIt& last, size_t n,
                std::bidirectional_iterator_tag)
    {
        for (; n != 0; n--)
        {
            if (first == last)
                return false;
            --last;
        }
        return true;
    }

    template <typename RndIt>
    bool recede(RndIt first, RndIt& last, size_t n,
                std::random_access_iterator_tag)
    {
        auto len = std::distance(first, last);
        assert(len >= 0);
        if ((size_t)len < n)
            return false;
        last -= n;
        return true;
    }

    template <typename It>
    bool recede(It first, It& last, size_t n = 1)
    {
        return recede(first, last, n,
               typename std::iterator_traits<It>::iterator_category());
    }
}}
