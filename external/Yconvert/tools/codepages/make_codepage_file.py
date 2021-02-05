#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-05-27.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
import codegen
import datetime
import os
import sys


FILE_TEMPLATE = """\
//****************************************************************************
// Copyright © [[[year]]] Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on [[[date]]]
//
// This file is distributed under the Simplified BSD License.
// License text is included with the source distribution.
//****************************************************************************
#include <array>
#include "Yconvert/Encoding.hpp"

namespace Yconvert
{
    struct CodePageRange
    {
        uint16_t startCodePoint;
        uint8_t startIndex;
        uint8_t length;
    };
    
    [[[arrays]]]
}
"""


GET_RANGES_TEMPLATE = """\
inline std::pair<const CodePageRange*, size_t>
get[[[label]]]CodePageRanges(Encoding encoding)
{
    switch (encoding)
    {
    [[[cases]]]
    default:
        return {nullptr, 0};
    }
}
"""


CASE_TEMPLATE = """\
case Encoding::[[[name]]]:
    return {[[[name]]]_CHARS, std::size([[[name]]]_CHARS)};\
"""


def get_mapping(line):
    line = line.lstrip()
    if not line or line[0] == "#":
        return None
    parts = line.split("\t", maxsplit=2)
    if len(parts) < 2:
        return None
    index = int(parts[0], 0)
    assert index <= 0xFF
    if parts[1] and not parts[1].isspace():
        codepoint = int(parts[1], 0)
        assert codepoint <= 0xFFFFF
    else:
        return None
    return index, codepoint


def get_char_ranges(char_mapping):
    if not char_mapping:
        return None
    ranges = [[char_mapping[0][0], char_mapping[0][1], 1]]
    for index, char in char_mapping[1:]:
        count = ranges[-1][2]
        if index == ranges[-1][0] + count and char == ranges[-1][1] + count:
            ranges[-1][2] += 1
        else:
            ranges.append([index, char, 1])
    return ranges


def read_character_map(file_name):
    mapping = []
    for line in open(file_name):
        values = get_mapping(line)
        if values:
            mapping.append(values)
    return mapping


def read_config_file(file_name):
    root = os.path.dirname(os.path.abspath(file_name))
    config = []
    for line in open(file_name):
        line = line.strip()
        if not line:
            continue
        parts = line.split(";")
        assert len(parts) == 2
        path = os.path.join(root, parts[1])
        config.append((parts[0], path))
    return config


def main(args):
    if len(args) != 1:
        print("usage: %s <configuration file>" % sys.argv[0])
        return 1
    config = read_config_file(args[0])
    arrays = {}
    for label, path in config:
        print(f"{label} {path}", file=sys.stderr)
        name = os.path.basename(path)
        name = os.path.splitext(name)[0].replace("-", "_")
        mapping = read_character_map(path)
        ranges = get_char_ranges(mapping)
        if not ranges:
            continue
        ranges.sort(key=lambda v: v[1])
        lines = [f"constexpr CodePageRange {label}_{name}_CHARS[] = {{"]
        strs = []
        for index, char, count in ranges:
            count -= 1
            if char < 65536:
                strs.append(f"{{0x{char:04X}, {index:3d}, {count:3d}}}")
            else:
                first = (char >> 16) & 0xFFFF
                second = char & 0xFFFF
                strs.append(f"{{0x{first:04X}, 255, 255}}")
                strs.append(f"{{0x{second:04X}, {index:3d}, {count:3d}}}")
        value_lines = codegen.join(strs, line_width=70, sep=", ", newline_sep=",")
        lines.extend("    " + s for s in value_lines)
        lines.append("};")
        entry = arrays.setdefault(label, ([], []))
        entry[0].append(lines)
        entry[1].append(f"{label}_{name}")
    lines = []
    for key in arrays:
        lines.append("")
        lines.append(f"#ifdef YCONVERT_ENABLE_{key}_CODE_PAGES")
        for arr_lines in arrays[key][0]:
            lines.append("")
            lines.extend(arr_lines)
        lines.append("")
        cases = []
        for c in arrays[key][1]:
            expander = codegen.DictExpander(dict(name=c))
            cases.extend(codegen.make_lines(CASE_TEMPLATE, expander))
        expander = codegen.DictExpander(dict(cases=cases, label=key.title()))
        lines.extend(codegen.make_lines(GET_RANGES_TEMPLATE, expander))
        lines.append("#endif")
    date = datetime.date.today()
    expander = codegen.DictExpander(dict(
        arrays=lines, year=date.year,
        date="%d-%02d-%02d" % (date.year, date.month, date.day)))
    print(codegen.make_text(FILE_TEMPLATE, expander))
    return 0


sys.exit(main(sys.argv[1:]))
