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
    class Range
    {
    public:
        typedef IteratorT Iterator;
        typedef std::iterator_traits<Iterator> IteratorTraits;
        typedef typename IteratorTraits::value_type ValueType;
        typedef typename IteratorTraits::reference Reference;
        typedef typename IteratorTraits::difference_type DifferenceType;

        Range()
        {}

        Range(Iterator first, Iterator last)
            : m_Range(first, last)
        {}

        explicit Range(std::pair<Iterator, Iterator> range)
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
    Iterator begin(Range<Iterator> range)
    {
        return range.begin();
    }

    template <typename Iterator>
    Iterator end(Range<Iterator> range)
    {
        return range.end();
    }

    template <typename Iterator>
    bool isEmpty(const Range<Iterator>& range)
    {
        return begin(range) == end(range);
    }

    template <typename Iterator>
    Range<Iterator> makeRange(Iterator first, Iterator last)
    {
        return Range<Iterator>(first, last);
    }

    template <typename Container>
    auto makeRange(const Container& c) -> Range<decltype(begin(c))>
    {
        return makeRange(begin(c), end(c));
    }

    template <typename Container>
    auto makeRange(Container& c) -> Range<decltype(begin(c))>
    {
        return makeRange(begin(c), end(c));
    }

    template <typename T, size_t N>
    Range<T*> makeArrayRange(T (&a)[N])
    {
        return makeRange<T*>(a, a + N - (a[N - 1] ? 0 : 1));
    }

    template <typename T>
    Range<T*> makeRange(T* s)
    {
        return makeRange(s, s + std::char_traits<T>::length(s));
    }

    template <typename Iterator>
    Range<Iterator> makeRange(std::pair<Iterator, Iterator> p)
    {
        return Range<Iterator>(p);
    }

    template <typename Iterator>
    Range<std::reverse_iterator<Iterator>>
    makeReverseRange(Range<Iterator> range)
    {
        return makeRange(std::reverse_iterator<Iterator>(range.end()),
                         std::reverse_iterator<Iterator>(range.begin()));
    }

    template <typename Iterator>
    size_t getSize(const Range<Iterator>& range)
    {
        return static_cast<size_t>(std::distance(begin(range), end(range)));
    }

    template <typename Container, typename Iterator>
    Container fromRange(Range<Iterator> range)
    {
        return Container(begin(range), end(range));
    }
}
