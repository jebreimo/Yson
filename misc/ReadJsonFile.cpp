//****************************************************************************
// Copyright © 2015 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 27.09.15
//
// This file is distributed under the Zero-Clause BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include "Yson/JsonReader.hpp"
#include <fstream>
#include <iostream>

using namespace Yson;

void readArray(JsonReader& reader);

void readObject(JsonReader& reader)
{
    reader.enter();
    while (reader.nextKey())
    {
        std::string key;
        reader.read(key);
        std::cout << key;
        if (reader.nextValue())
        {
            switch (reader.valueType())
            {
            case ValueType::STRING:
            {
                std::string value;
                reader.read(value);
                std::cout << ": " << value;
                break;
            }
            case ValueType::INTEGER:
            {
                int64_t value;
                reader.read(value);
                std::cout << ": " << value;
                break;
            }
            case ValueType::ARRAY:
                readArray(reader);
                break;
            default:
                break;
            }
        }
        std::cout << "\n";
    }
    reader.leave();
}

void readArray(JsonReader& reader)
{
    reader.enter();
    while (reader.nextValue())
    {
        switch (reader.valueType())
        {
        case ValueType::STRING:
        {
            std::string value;
            reader.read(value);
            std::cout << " " << value;
            break;
        }
        case ValueType::INTEGER:
        {
            int64_t value;
            reader.read(value);
            std::cout << " " << value;
            break;
        }
        case ValueType::ARRAY:
            readArray(reader);
            break;
        default:
            break;
        }
        std::cout << "\n";
    }
    reader.leave();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <json file>\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    JsonReader reader(file);
    if (!reader.nextValue())
        return 0;
    switch (reader.valueType())
    {
    case ValueType::OBJECT:
        readObject(reader);
        break;
    case ValueType::ARRAY:
        readArray(reader);
        break;
    default:
        break;
    }

    return 0;
}
