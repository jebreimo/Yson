//****************************************************************************
// Copyright © 2021 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2021-06-02.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include "Reader.hpp"

namespace Yson
{
    namespace detail
    {
        YSON_API bool initialize(Reader& reader);
    }

    /**
     * Provides an input iterator for iterating over the keys in an object.
     * It automatically skips the object's opening brace and stops after the
     * closing brace. Access to the actual values is done through the Reader
     * object.
     */
    class YSON_API ObjectKeyIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = void;
        using pointer = const std::string*;
        using reference = const std::string&;

        ObjectKeyIterator();

        explicit ObjectKeyIterator(Reader& reader);

        [[nodiscard]]
        const std::string& operator*() const;

        [[nodiscard]]
        const std::string* operator->() const;

        ObjectKeyIterator& operator++();

        friend bool operator==(const ObjectKeyIterator& a,
                               const ObjectKeyIterator& b);

        [[nodiscard]]
        ObjectKeyIterator& begin();

        [[nodiscard]]
        static ObjectKeyIterator end();
    private:
        Reader* m_Reader;
        std::string m_Key;
        bool m_AtEnd;
    };

    YSON_API bool operator!=(const ObjectKeyIterator& a,
                             const ObjectKeyIterator& b);

    /**
     * A convenience function for creating an ObjectKeyIterator.
     *
     * @param reader An instance of Reader positioned at the opening brace of
     *  an object.
     * @return A new ObjectKeyIterator.
     */
    inline ObjectKeyIterator keys(Reader& reader)
    {
        return ObjectKeyIterator(reader);
    }

    /**
     * Provides an input iterator for iterating over the values in an array.
     * It automatically skips the array's opening bracket and stops after
     * the closing bracket. Access to the actual values is done through
     * the Reader object.
     */
    class YSON_API ArrayIterator
    {
    public:
        explicit ArrayIterator(Reader& reader);

        bool next();
    private:
        Reader* m_Reader;
        int m_State;
    };

    /**
     * Provides an input iterator for iterating over the values in an array.
     * It automatically skips the array's opening bracket and stops after
     * the closing bracket.
     * @tparam T The type of the values in the array.
     */
    template <typename T>
    class ArrayValueIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = void;
        using pointer = const T*;
        using reference = const T&;

        ArrayValueIterator()
            : m_Reader(),
              m_Value(),
              m_AtEnd(true)
        {}

        explicit ArrayValueIterator(Reader& reader)
            : m_Reader(&reader),
              m_Value(),
              m_AtEnd(!detail::initialize(*m_Reader))
        {
            if (!m_AtEnd)
                operator++();
        }

        [[nodiscard]]
        const T& operator*() const
        {
            return m_Value;
        }

        [[nodiscard]]
        const T* operator->() const
        {
            return &m_Value;
        }

        ArrayValueIterator& operator++()
        {
            if (!m_AtEnd)
            {
                if (m_Reader->nextValue())
                {
                    m_Value = read<T>(*m_Reader);
                }
                else
                {
                    m_Reader->leave();
                    m_AtEnd = true;
                }
            }

            return *this;
        }

        friend bool operator==(const ArrayValueIterator& a,
                               const ArrayValueIterator& b)
        {
            return &a == &b || (a.m_AtEnd && b.m_AtEnd);
        }

        friend bool operator!=(const ArrayValueIterator& a,
                               const ArrayValueIterator& b)
        {
            return !(a == b);
        }

        ArrayValueIterator& begin()
        {
            return *this;
        }

        static ArrayValueIterator end()
        {
            return {};
        }
    private:
        Reader* m_Reader;
        T m_Value;
        bool m_AtEnd;
    };

    /**
     * A convenience function for creating an ArrayValueIterator.
     *
     * @tparam T the type of the values in the array.
     * @param reader an instance of Reader positioned at the opening bracket
     *  of an array.
     * @return A new ArrayValueIterator.
     */
    template <typename T>
    ArrayValueIterator<T> arrayValues(Reader& reader)
    {
        return ArrayValueIterator<T>(reader);
    }
}
