#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

if [ ! -e "$1" ]; then
  echo "Usage: $0 <code page configuration>"
  exit 1
fi

echo Making CodePageDefinitions.hpp
python3 "$SCRIPT_DIR/make_codepage_file.py" $1 >CodePageDefinitions.hpp
