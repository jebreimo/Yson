#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ===========================================================================
# Copyright © 2020 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2020-03-05.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
import argparse
import glob
import os
import re
import sys


def make_argument_parser():
    ap = argparse.ArgumentParser(
        description='List files included by a C++ file.')
    ap.add_argument("files", metavar="C++ files", nargs="+",
                    help="The C++ files that are to be analyzed.")
    ap.add_argument("-f", "--filter", metavar="FILE", action="append",
                    help="Filter out FILE or files in FILE if FILE is a directory.")
    ap.add_argument("-i", "--include", metavar="DIR", action="append",
                    help="Include directory.")
    return ap


def get_all_files_names(dir_name):
    result = {}
    for top_dir, subdirs, file_names in os.walk(dir_name):
        for file in file_names:
            path = os.path.join(top_dir, file)
            relpath = os.path.relpath(path, dir_name)
            abspath = os.path.realpath(path)
            if relpath not in result:
                result[relpath] = abspath
    return result


def get_includes(file_path):
    regex = re.compile(r"\s*#include\s*(?:\"([^\"]+)\"|<([^>]+)>).*")
    includes = []
    for line in open(file_path):
        m = regex.match(line)
        if m:
            print(m.groups())
            includes.append(m.group(1) or m.group(2))
    return includes


def get_dependency_map(files, known_files):
    dependencies = {}

    next_files_to_check = [os.path.realpath(f) for f in files]

    while next_files_to_check:
        files_to_check = next_files_to_check
        next_files_to_check = []

        for file_path in files_to_check:
            real_file_path = os.path.realpath(file_path)
            if real_file_path in dependencies:
                continue

            current_known_files = known_files.copy()
            dir_name = os.path.dirname(file_path)
            if not dir_name:
                dir_name = os.getcwd()
            current_known_files.update(get_all_files_names(dir_name))

            dependencies[real_file_path] = []
            visited = set()

            for include in get_includes(file_path):
                real_include_path = current_known_files.get(include)
                if not real_include_path or real_include_path in visited:
                    continue

                visited.add(real_include_path)
                dependencies[real_file_path].append(real_include_path)
                if real_include_path not in dependencies:
                    next_files_to_check.append(real_include_path)
    return dependencies


def print_includes_rec(file_path, dependencies, visited, level=0):
    visited.add(file_path)
    for dependency in dependencies[file_path]:
        if dependency not in visited:
            print_includes_rec(dependency, dependencies, visited, level + 1)
    print(("  " * level) + file_path)


def print_includes(file_path, dependencies):
    print_includes_rec(os.path.realpath(file_path), dependencies, set())


def get_file_inclusion_order_rec(result, file_path, dependencies, visited):
    visited.add(file_path)
    for dependency in dependencies[file_path]:
        if dependency not in visited:
            get_file_inclusion_order_rec(result, dependency,
                                         dependencies, visited)
    result.append(file_path)


def get_file_inclusion_order(file_paths, dependencies, ignore_files):
    visited = set(ignore_files)
    result = []
    for file_path in file_paths:
        get_file_inclusion_order_rec(result, os.path.realpath(file_path),
                                     dependencies, visited)
    return result


def main():
    args = make_argument_parser().parse_args()

    known_files = {}
    if args.include:
        for dir_name in args.include[::-1]:
            if os.path.isdir(dir_name):
                known_files.update(get_all_files_names(dir_name))

    ignore_files = set()
    if args.filter:
        for name in args.filter:
            if os.path.isdir(name):
                ignore_files.update(get_all_files_names(name).values())
            elif os.path.isfile(name):
                ignore_files.add(os.path.realpath(name))

    dependencies = get_dependency_map(args.files, known_files)

    # for file in args.files:
    #     print_includes(file, dependencies)

    for file in get_file_inclusion_order(args.files, dependencies, ignore_files):
        print(file)


if __name__ == "__main__":
    sys.exit(main())
