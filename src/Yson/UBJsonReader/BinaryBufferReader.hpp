//****************************************************************************
// Copyright © 2017 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 06.03.2017.
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once
#include <vector>
#include "BinaryReader.hpp"

namespace Yson
{
    class BinaryBufferReader : public BinaryReader
    {
    public:
        BinaryBufferReader(const char* buffer, size_t size);

        bool advance(size_t count) override;

        [[nodiscard]] const void* data() const override;

        [[nodiscard]] char front() const override;

        [[nodiscard]] size_t position() const override;

        bool peek(char* value) override;

        bool read(size_t size) override;

        size_t size() override;

        bool read(void* buffer, size_t size, size_t unitSize) override;

    private:
        const char* m_TokenStart;
        const char* m_TokenEnd;
        const char* m_BufferStart;
        const char* m_BufferEnd;
    };
}
