#!/bin/bash

YSON_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ ! -d release.Yson ]
then
    mkdir release.Yson
fi

if [ $# == 1 ]
then
    set INSTALL_DIR="-DCMAKE_PREFIX_PATH=$1"
else
    set INSTALL_DIR=
fi

cd release.Yson
cmake -DCMAKE_BUILD_TYPE=Release $INSTALL_DIR $YSON_DIR
make install
cd ..

if [ ! -d debug.Yson ]
then
    mkdir debug.Yson
fi

cd debug.Yson
cmake -DCMAKE_BUILD_TYPE=Debug $INSTALL_DIR $YSON_DIR
make install
cd ..
