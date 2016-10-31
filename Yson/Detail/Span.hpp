//****************************************************************************
// Copyright © 2013 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2013-07-01
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include <iterator>
#include <utility>

namespace Yson
{
    template <typename IteratorT>
    class Span
    {
    public:
        typedef IteratorT Iterator;
        typedef std::iterator_traits<Iterator> IteratorTraits;
        typedef typename IteratorTraits::value_type ValueType;
        typedef typename IteratorTraits::reference Reference;
        typedef typename IteratorTraits::difference_type DifferenceType;

        Span()
        {}

        Span(Iterator first, Iterator last)
            : m_Range(first, last)
        {}

        explicit Span(std::pair<Iterator, Iterator> range)
            : m_Range(range)
        {}

        operator std::pair<Iterator, Iterator>()
        {
            return m_Range;
        }

        Iterator& begin()
        {
            return m_Range.first;
        }

        Iterator begin() const
        {
            return m_Range.first;
        }

        Iterator& end()
        {
            return m_Range.second;
        }

        Iterator end() const
        {
            return m_Range.second;
        }

        Reference front() const
        {
            return *m_Range.first;
        }

        Reference back() const
        {
            using namespace std;
            return *prev(m_Range.first);
        }

        size_t size() const
        {
            return static_cast<size_t>(m_Range.second - m_Range.first);
        }
    private:
        std::pair<Iterator, Iterator> m_Range;
    };

    template <typename Iterator>
    Iterator begin(Span<Iterator> range)
    {
        return range.begin();
    }

    template <typename Iterator>
    Iterator end(Span<Iterator> range)
    {
        return range.end();
    }

    template <typename Iterator>
    bool isEmpty(const Span<Iterator>& range)
    {
        return begin(range) == end(range);
    }

    template <typename Iterator>
    Span<Iterator> makeSpan(Iterator first, Iterator last)
    {
        return Span<Iterator>(first, last);
    }

    template <typename Container>
    auto makeSpan(const Container& c) -> Span<decltype(begin(c))>
    {
        return makeSpan(begin(c), end(c));
    }

    template <typename Container>
    auto makeSpan(Container& c) -> Span<decltype(begin(c))>
    {
        return makeSpan(begin(c), end(c));
    }

    template <typename T, size_t N>
    Span<T*> makeArraySpan(T (& a)[N])
    {
        return makeSpan<T*>(a, a + N - (a[N - 1] ? 0 : 1));
    }

    template <typename T>
    Span<T*> makeSpan(T* s)
    {
        return makeSpan(s, s + std::char_traits<T>::length(s));
    }

    template <typename Iterator>
    Span<Iterator> makeSpan(std::pair<Iterator, Iterator> p)
    {
        return Span<Iterator>(p);
    }

    template <typename Iterator>
    Span<std::reverse_iterator<Iterator>>
    makeReverseSpan(Span<Iterator> range)
    {
        return makeSpan(std::reverse_iterator<Iterator>(range.end()),
                         std::reverse_iterator<Iterator>(range.begin()));
    }

    template <typename Iterator>
    size_t getSize(const Span<Iterator>& range)
    {
        return static_cast<size_t>(std::distance(begin(range), end(range)));
    }

    template <typename Container, typename Iterator>
    Container fromSpan(Span<Iterator> range)
    {
        return Container(begin(range), end(range));
    }
}
