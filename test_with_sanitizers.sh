#!/bin/sh

export CC=clang-11
export CXX=clang++-11
export  CFLAGS="-g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"
export  CXXFLAGS="-g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"
export  LDFLAGS="-g -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"

make clean
make
make examples
make test
