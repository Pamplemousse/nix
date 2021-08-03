#!/usr/bin/env bash

BUILD_DIR="fuzz/build"

OPTIONS="\
    -Ddebug=true \
    -Doptimization=0 \
    -Denable_s3=disabled \
    -Denable_gc=disabled \
    --prefix=$(pwd)/outputs/out"

if [ ! -d $BUILD_DIR ]; then
    meson setup $BUILD_DIR $OPTIONS
else
    meson --reconfigure $BUILD_DIR $OPTIONS
fi

meson compile -C $BUILD_DIR -v \
  parse-fuzasan \
  parse_eval-fuzasan \
  parse_store_path-fuzasan \
  daemon-fuzasan
