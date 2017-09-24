//****************************************************************************
// Copyright © 2016 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 24.12.2016.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <cstddef>
#include <string>

#if defined(__clang__) && __clang_major__ >= 9
    #define YSON_STD_STRINGVIEW
#endif

#if defined(YSON_STD_STRINGVIEW)
    #include <string_view>
#elif defined(YSON_EXPERIMENTAL_STRING_VIEW)
    #include <experimental/string_view>

    namespace std
    {
        using experimental::string_view;
    }
#else
    #include <algorithm>
    #include <cstring>
    #include <ostream>

    namespace Yson
    {
        /** @brief A minimal implementation of std::string_view for
          *     environments where this class is missing from the standard
          *     library.
          */
        class StringView
        {
        public:
            constexpr static size_t npos = SIZE_MAX;

            StringView()
                : StringView(nullptr, 0)
            {}

            StringView(const char* data)
                : StringView(data, std::strlen(data))
            {}

            StringView(const char* data, size_t size)
                : m_Data(data),
                  m_Size(size)
            {}

            const char* begin() const
            {
                return m_Data;
            }

            const char* end() const
            {
                return m_Data + m_Size;
            }

            char operator[](size_t index) const
            {
                return m_Data[index];
            }

            const char* data() const
            {
                return m_Data;
            }

            bool empty() const
            {
                return m_Size == 0;
            }

            size_t size() const
            {
                return m_Size;
            }

            size_t find_last_not_of(char c) const
            {
                for (auto i = m_Size; i-- > 0;)
                {
                    if (m_Data[i] != c)
                        return i;
                }
                return npos;
            }

            StringView substr(size_t offset = 0, size_t length = SIZE_MAX) const
            {
                offset = std::min(offset, m_Size);
                return StringView(m_Data + offset,
                                  std::min(length, m_Size - offset));
            }
        private:
            const char* m_Data;
            size_t m_Size;
        };

        inline bool operator==(StringView a, const char* b)
        {
            return a.size() == std::strlen(b)
                   && std::memcmp(a.data(), b, a.size()) == 0;
        }

        inline bool operator==(const StringView& a, const std::string& b)
        {
            return a.size() == b.size()
                   && std::memcmp(a.data(), b.data(), a.size()) == 0;
        }

        inline std::ostream& operator<<(std::ostream& os, StringView str)
        {
            return os.write(str.data(), str.size());
        }
    }

    namespace std
    {
        using string_view = Yson::StringView;
    }
#endif

namespace Yson
{
    inline std::string_view makeStringView(const char* from, const char* to)
    {
        return std::string_view(from, to - from);
    }

    inline std::string_view makeStringView(const std::string& str)
    {
        return std::string_view(str.data(), str.size());
    }
}
