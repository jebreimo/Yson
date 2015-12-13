#!/bin/bash

YSON_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ ! -d release.Yson ]
then
    mkdir release.Yson
fi

cd release.Yson
cmake -DCMAKE_BUILD_TYPE=Release $YSON_DIR
make install
cd ..

if [ ! -d debug.Yson ]
then
    mkdir debug.Yson
fi

cd debug.Yson
cmake -DCMAKE_BUILD_TYPE=Debug $YSON_DIR
make install
cd ..
