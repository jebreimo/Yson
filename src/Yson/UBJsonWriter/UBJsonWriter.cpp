//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/UBJsonWriter.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <Yconvert/Convert.hpp>
#include "Yson/YsonException.hpp"
#include "Yson/Common/Base64.hpp"
#include "Yson/Common/GetUnicodeFileName.hpp"
#include "UBJsonWriterUtilities.hpp"

namespace Yson
{
    namespace
    {
        constexpr size_t MAX_BUFFER_SIZE = 64 * 1024;

        struct Context
        {
            Context() = default;

            Context(UBJsonValueType structureType,
                    ptrdiff_t size,
                    UBJsonValueType valueType = UBJsonValueType::UNKNOWN)
                    : index(0),
                      size(size),
                      structureType(structureType),
                      valueType(valueType)
            {}

            ptrdiff_t index = 0;
            ptrdiff_t size = -1;
            UBJsonValueType structureType = UBJsonValueType::UNKNOWN;
            UBJsonValueType valueType = UBJsonValueType::UNKNOWN;
        };
    }

    struct UBJsonWriter::Members
    {
        std::unique_ptr<std::ostream> streamPtr;
        std::ostream* stream = nullptr;
        mutable std::vector<char> buffer;
        std::string key;
        std::stack<Context> contexts;
        size_t maxBufferSize = MAX_BUFFER_SIZE;
        bool strictIntegerSizes = false;
    };

    UBJsonWriter::UBJsonWriter()
        : UBJsonWriter(std::unique_ptr<std::ostream>(), nullptr)
    {}

    UBJsonWriter::UBJsonWriter(const std::string& fileName)
        : UBJsonWriter(std::make_unique<std::ofstream>(getUnicodeFileName(fileName),
                                                       std::ios_base::binary),
                       nullptr)
    {}

    UBJsonWriter::UBJsonWriter(std::ostream& stream)
        : UBJsonWriter(std::unique_ptr<std::ostream>(), &stream)
    {}

    UBJsonWriter::UBJsonWriter(std::unique_ptr<std::ostream> streamPtr,
                               std::ostream* stream)
        : m_Members(std::make_unique<Members>())
    {
        m_Members->streamPtr = move(streamPtr);
        m_Members->stream =  m_Members->streamPtr
                             ? m_Members->streamPtr.get()
                             : stream;
        m_Members->contexts.push(Context());
        m_Members->buffer.reserve(MAX_BUFFER_SIZE);
        if (!m_Members->stream)
            m_Members->maxBufferSize = SIZE_MAX;
    }

    UBJsonWriter::UBJsonWriter(UBJsonWriter&& other) noexcept
        : m_Members(move(other.m_Members))
    {}

    UBJsonWriter::~UBJsonWriter()
    {
        UBJsonWriter::flush();
    }

    UBJsonWriter& UBJsonWriter::operator=(UBJsonWriter&& other) noexcept
    {
        m_Members = move(other.m_Members);
        return *this;
    }

    std::ostream* UBJsonWriter::stream()
    {
        flush();
        return members().stream;
    }

    std::pair<const void*, size_t> UBJsonWriter::buffer() const
    {
        auto& m = members();
        if (m.stream)
            return {nullptr, 0};
        return {m.buffer.data(), m.buffer.size()};
    }

    const std::string& UBJsonWriter::key() const
    {
        return members().key;
    }

    UBJsonWriter& UBJsonWriter::key(std::string key)
    {
        members().key = move(key);
        return *this;
    }

    UBJsonWriter& UBJsonWriter::beginArray()
    {
        return beginStructure(UBJsonValueType::ARRAY, UBJsonParameters());
    }

    UBJsonWriter& UBJsonWriter::beginArray(const StructureParameters& params)
    {
        return beginStructure(UBJsonValueType::ARRAY,
                              params.ubJsonParameters);
    }

    UBJsonWriter& UBJsonWriter::endArray()
    {
        return endStructure(UBJsonValueType::ARRAY);
    }

    UBJsonWriter& UBJsonWriter::beginObject()
    {
        return beginStructure(UBJsonValueType::OBJECT, UBJsonParameters());
    }

    UBJsonWriter& UBJsonWriter::beginObject(
            const StructureParameters& parameters)
    {
        return beginStructure(UBJsonValueType::OBJECT,
                              parameters.ubJsonParameters);
    }

    UBJsonWriter& UBJsonWriter::endObject()
    {
        return endStructure(UBJsonValueType::OBJECT);
    }

    UBJsonWriter& UBJsonWriter::null()
    {
        beginValue();
        auto& m = members();
        if (m.contexts.top().valueType == UBJsonValueType::UNKNOWN)
            m.buffer.push_back('Z');
        return *this;
    }

    UBJsonWriter& UBJsonWriter::boolean(bool value)
    {
        beginValue();
        auto& m = members();
        if (m.contexts.top().valueType == UBJsonValueType::UNKNOWN)
            m.buffer.push_back(value ? 'T' : 'F');
        return *this;
    }

    UBJsonWriter& UBJsonWriter::value(char value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(signed char value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(short value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(long value)
    {
        return writeInteger(static_cast<int64_t>(value));
    }

    UBJsonWriter& UBJsonWriter::value(long long value)
    {
        return writeInteger(static_cast<int64_t>(value));
    }

    UBJsonWriter& UBJsonWriter::value(unsigned char value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(unsigned short value)
    {
        if (value <= INT16_MAX)
            return writeInteger(static_cast<int16_t>(value));
        else if (!members().strictIntegerSizes)
            return writeInteger<int32_t>(value);
        YSON_THROW("uint16_t value " + std::to_string(value)
                   + " is greater than INT16_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(unsigned value)
    {
        if (value <= INT32_MAX)
            return writeInteger(static_cast<int32_t>(value));
        else if (!members().strictIntegerSizes)
            return writeInteger<int64_t>(value);
        YSON_THROW("uint32_t value " + std::to_string(value)
                   + " is greater than INT32_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(unsigned long value)
    {
        if (value <= INT64_MAX)
            return writeInteger(static_cast<int64_t>(value));
        YSON_THROW("uint64_t value " + std::to_string(value)
                   + " is greater than INT64_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(unsigned long long value)
    {
        if (value <= INT64_MAX)
            return writeInteger(static_cast<int64_t>(value));
        YSON_THROW("uint64_t value " + std::to_string(value)
                   + " is greater than INT64_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(float value)
    {
        return writeFloat(value);
    }

    UBJsonWriter& UBJsonWriter::value(double value)
    {
        return writeFloat(value);
    }

    UBJsonWriter& UBJsonWriter::value(std::string_view text)
    {
        beginValue();
        auto& m = members();
        auto& context = m.contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            m.buffer.push_back('S');
        writeMinimalInteger(m.buffer, text.size());
        m.buffer.insert(m.buffer.end(), text.begin(), text.end());
        return *this;
    }

    UBJsonWriter& UBJsonWriter::value(std::wstring_view text)
    {
        return value(Yconvert::convertTo<std::string>(
                text,
                Yconvert::Encoding::UTF_16_NATIVE,
                Yconvert::Encoding::UTF_8));
    }

    UBJsonWriter& UBJsonWriter::binary(const void* data, size_t size)
    {
        beginArray(UBJsonParameters(size, UBJsonValueType::UINT_8));
        flush();
        auto& m = members();
        m.stream->write(static_cast<const char*>(data), size);
        m.contexts.top().index = size;
        return endArray();
    }

    UBJsonWriter& UBJsonWriter::base64(const void* data, size_t size)
    {
        return value(toBase64(data, size));
    }

    UBJsonWriter& UBJsonWriter::noop()
    {
        members().buffer.push_back('N');
        flush();
        return *this;
    }

    bool UBJsonWriter::isStrictIntegerSizesEnabled() const
    {
        return members().strictIntegerSizes;
    }

    UBJsonWriter& UBJsonWriter::setStrictIntegerSizesEnabled(bool value)
    {
        members().strictIntegerSizes = value;
        return *this;
    }

    UBJsonWriter::Members& UBJsonWriter::members() const
    {
        if (m_Members)
            return *m_Members;
        YSON_THROW("The members of this UBJsonWriter instance have been moved to another instance.");
    }

    UBJsonWriter& UBJsonWriter::beginStructure(
            UBJsonValueType structureType,
            const UBJsonParameters& parameters)
    {
        assert(structureType == UBJsonValueType::OBJECT
               || structureType == UBJsonValueType::ARRAY);

        beginValue();
        auto& m = members();
        auto& context = m.contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            m.buffer.push_back(char(structureType));
        if (parameters.size >= 0)
        {
            if (parameters.valueType != UBJsonValueType::UNKNOWN)
            {
                m.buffer.push_back('$');
                m.buffer.push_back(char(parameters.valueType));
            }
            m.buffer.push_back('#');
            writeMinimalInteger(m.buffer, int64_t(parameters.size));
        }
        m.contexts.push(Context(structureType,
                                 parameters.size,
                                 parameters.valueType));
        return *this;
    }

    UBJsonWriter& UBJsonWriter::endStructure(UBJsonValueType structureType)
    {
        auto endChar = structureType == UBJsonValueType::OBJECT ? '}' : ']';
        auto& m = members();
        auto& context = m.contexts.top();
        if (context.structureType == structureType)
        {
            if (context.size == -1)
            {
                m.buffer.push_back(endChar);
            }
            else if (context.index != context.size)
            {
                if (context.index != 0
                    || (context.valueType != UBJsonValueType::NULL_VALUE
                        && context.valueType != UBJsonValueType::FALSE_VALUE
                        && context.valueType != UBJsonValueType::TRUE_VALUE))
                {
                    YSON_THROW("Too few items in optimized array. Has "
                               + std::to_string(context.index) + ", expects "
                               + std::to_string(context.size) + ".");
                }
            }
        }
        // This function could be called from the destructor of some
        // RAII-class. If so, make sure that we're not already processing
        // another exception before throwing our own.
        else if (!std::uncaught_exceptions())
        {
            YSON_THROW("Ending structure " + toString(context.structureType)
                       + " as if it was a " + toString(structureType));
        }
        m.contexts.pop();
        return *this;
    }

    void UBJsonWriter::beginValue()
    {
        auto& m = members();
        if (m.buffer.size() >= m.maxBufferSize)
            flush();
        auto& context = m.contexts.top();
        auto& key = m.key;
        if (context.structureType == UBJsonValueType::OBJECT)
        {
            writeMinimalInteger(m.buffer, m.key.size());
            m.buffer.insert(m.buffer.end(), key.begin(), key.end());
        }
        key.clear();
        if (context.index == context.size)
            YSON_THROW("Already at the end of optimized object or array.");
        ++context.index;
    }

    UBJsonWriter& UBJsonWriter::flush()
    {
        auto& m = members();
        if (m.stream && !m.buffer.empty())
        {
            m.stream->write(m.buffer.data(), m.buffer.size());
            m.buffer.clear();
        }
        return *this;
    }

    template <typename T>
    UBJsonWriter& UBJsonWriter::writeFloat(T value)
    {
        beginValue();
        auto& m = members();
        auto& context = m.contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            writeValueWithMarker(m.buffer, value);
        else
            writeFloatAs(m.buffer, value, context.valueType);
        return *this;
    }

    template <typename T>
    UBJsonWriter& UBJsonWriter::writeInteger(T value)
    {
        beginValue();
        auto& m = members();
        auto& context = m.contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
        {
            if (!m.strictIntegerSizes)
                writeMinimalInteger(m.buffer, value);
            else
                writeValueWithMarker(m.buffer, value);
        }
        else
        {
            writeIntegerAs(m.buffer, value, context.valueType);
        }
        return *this;
    }
}
