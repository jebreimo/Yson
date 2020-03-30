#!/bin/bash

YSON_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

if [ ! -d release.Yson ]; then
  mkdir -p build/release.Yson
fi

if [ $# == 1 ]; then
  set INSTALL_DIR="-DCMAKE_INSTALL_PREFIX=$1"
else
  set INSTALL_DIR=
fi

pushd .
cd build/release.Yson \
&& cmake -DCMAKE_BUILD_TYPE=Release $INSTALL_DIR $YSON_DIR -- -j 2 \
&& make install
popd

if [ ! -d debug.Yson ]; then
  mkdir -p build/debug.Yson
fi

pushd .
cd build/debug.Yson \
&& cmake -DCMAKE_BUILD_TYPE=Debug $INSTALL_DIR $YSON_DIR \
&& make install
popd
