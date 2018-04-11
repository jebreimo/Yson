//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 18.03.2017.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "../UBJsonWriter.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include "../../Ystring/Conversion.hpp"
#include "../YsonException.hpp"
#include "../Common/Base64.hpp"
#include "../Common/GetUnicodeFileName.hpp"
#include "UBJsonWriterUtilities.hpp"

namespace Yson
{
    namespace
    {
        size_t MAX_BUFFER_SIZE = 1024 * 1024;

        struct Context
        {
            Context() {}

            Context(UBJsonValueType structureType)
                    : structureType(structureType)
            {}

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
        bool strictIntegerSizes = false;
    };

    UBJsonWriter::UBJsonWriter()
        : UBJsonWriter(std::cout)
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
                             : stream;;
        m_Members->contexts.push(Context());
        m_Members->buffer.reserve(MAX_BUFFER_SIZE);
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

    std::ostream& UBJsonWriter::stream()
    {
        flush();
        return *m_Members->stream;
    }

    const std::string& UBJsonWriter::key() const
    {
        return m_Members->key;
    }

    UBJsonWriter& UBJsonWriter::key(const std::string& key)
    {
        m_Members->key = key;
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
        if (m_Members->contexts.top().valueType == UBJsonValueType::UNKNOWN)
            m_Members->buffer.push_back('Z');
        return *this;
    }

    UBJsonWriter& UBJsonWriter::boolean(bool value)
    {
        beginValue();
        if (m_Members->contexts.top().valueType == UBJsonValueType::UNKNOWN)
            m_Members->buffer.push_back(value ? 'T' : 'F');
        return *this;
    }

    UBJsonWriter& UBJsonWriter::value(char value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int8_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int16_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int32_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(int64_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(uint8_t value)
    {
        return writeInteger(value);
    }

    UBJsonWriter& UBJsonWriter::value(uint16_t value)
    {
        if (value <= INT16_MAX)
            return writeInteger<int16_t>(value);
        else if (!m_Members->strictIntegerSizes)
            return writeInteger<int32_t>(value);
        YSON_THROW("uint16_t value " + std::to_string(value)
                   + " is greater than INT16_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(uint32_t value)
    {
        if (value <= INT32_MAX)
            return writeInteger<int32_t>(value);
        else if (!m_Members->strictIntegerSizes)
            return writeInteger<int64_t>(value);
        YSON_THROW("uint32_t value " + std::to_string(value)
                   + " is greater than INT32_MAX");
    }

    UBJsonWriter& UBJsonWriter::value(uint64_t value)
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

    UBJsonWriter& UBJsonWriter::value(const std::string& text)
    {
        beginValue();
        auto& context = m_Members->contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            m_Members->buffer.push_back('S');
        writeMinimalInteger(m_Members->buffer, text.size());
        m_Members->buffer.insert(m_Members->buffer.end(),
                                 text.begin(), text.end());
        return *this;
    }

    UBJsonWriter& UBJsonWriter::value(const std::wstring& text)
    {
        return value(Ystring::Conversion::convert<std::string>(
                text,
                Ystring::Encoding::UTF_16,
                Ystring::Encoding::UTF_8));
    }

    UBJsonWriter& UBJsonWriter::binary(const void* data, size_t size)
    {
        beginArray(UBJsonParameters(size, UBJsonValueType::UINT_8));
        flush();
        m_Members->stream->write(static_cast<const char*>(data), size);
        m_Members->contexts.top().index = size;
        return endArray();
    }

    UBJsonWriter& UBJsonWriter::base64(const void* data, size_t size)
    {
        return value(toBase64(data, size));
    }

    UBJsonWriter& UBJsonWriter::noop()
    {
        m_Members->buffer.push_back('N');
        flush();
        return *this;
    }

    bool UBJsonWriter::isStrictIntegerSizesEnabled() const
    {
        return m_Members->strictIntegerSizes;
    }

    UBJsonWriter& UBJsonWriter::setStrictIntegerSizesEnabled(bool value)
    {
        m_Members->strictIntegerSizes = value;
        return *this;
    }

    UBJsonWriter& UBJsonWriter::beginStructure(
            UBJsonValueType structureType,
            const UBJsonParameters& parameters)
    {
        assert(structureType == UBJsonValueType::OBJECT
               || structureType == UBJsonValueType::ARRAY);

        beginValue();
        auto& context = m_Members->contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            m_Members->buffer.push_back(char(structureType));
        if (parameters.size >= 0)
        {
            if (parameters.valueType != UBJsonValueType::UNKNOWN)
            {
                m_Members->buffer.push_back('$');
                m_Members->buffer.push_back(char(parameters.valueType));
            }
            m_Members->buffer.push_back('#');
            writeMinimalInteger(m_Members->buffer, int64_t(parameters.size));
        }
        m_Members->contexts.push(Context(structureType,
                                 parameters.size,
                                 parameters.valueType));
        return *this;
    }

    UBJsonWriter& UBJsonWriter::endStructure(UBJsonValueType structureType)
    {
        auto endChar = structureType == UBJsonValueType::OBJECT ? '}' : ']';
        auto& context = m_Members->contexts.top();
        if (context.structureType == structureType)
        {
            if (context.size == -1)
            {
                m_Members->buffer.push_back(endChar);
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
        else if (!std::uncaught_exception())
        {
            YSON_THROW("Ending structure " + toString(context.structureType)
                       + " as if it was a " + toString(structureType));
        }
        m_Members->contexts.pop();
        return *this;
    }

    void UBJsonWriter::beginValue()
    {
        if (m_Members->buffer.size() >= MAX_BUFFER_SIZE)
            flush();
        auto& context = m_Members->contexts.top();
        auto& key = m_Members->key;
        if (context.structureType == UBJsonValueType::OBJECT)
        {
            writeMinimalInteger(m_Members->buffer, m_Members->key.size());
            m_Members->buffer.insert(m_Members->buffer.end(),
                                     key.begin(), key.end());
        }
        key.clear();
        if (context.index == context.size)
            YSON_THROW("Already at the end of optimized object or array.");
        ++context.index;
    }

    UBJsonWriter& UBJsonWriter::flush()
    {
        if (m_Members->stream && !m_Members->buffer.empty())
        {
            m_Members->stream->write(m_Members->buffer.data(),
                                     m_Members->buffer.size());
            m_Members->buffer.clear();
        }
        return *this;
    }

    template <typename T>
    UBJsonWriter& UBJsonWriter::writeFloat(T value)
    {
        beginValue();
        auto& context = m_Members->contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
            writeValueWithMarker(m_Members->buffer, value);
        else
            writeFloatAs(m_Members->buffer, value, context.valueType);
        return *this;
    }

    template <typename T>
    UBJsonWriter& UBJsonWriter::writeInteger(T value)
    {
        beginValue();
        auto& context = m_Members->contexts.top();
        if (context.valueType == UBJsonValueType::UNKNOWN)
        {
            if (!m_Members->strictIntegerSizes)
                writeMinimalInteger(m_Members->buffer, value);
            else
                writeValueWithMarker(m_Members->buffer, value);
        }
        else
        {
            writeIntegerAs(m_Members->buffer, value, context.valueType);
        }
        return *this;
    }
}
