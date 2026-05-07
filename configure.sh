#!/usr/bin/env bash

cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DLLVM_EXTERNAL_PROJECTS=mambo -DLLVM_EXTERNAL_MAMBO_SOURCE_DIR=../mambo-lisp -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../llvm-project/llvm -B build
