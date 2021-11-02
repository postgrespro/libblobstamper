#!/bin/sh

export CC=clang-11
export CXX=clang++-11
export  CFLAGS="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"
export  CXXFLAGS="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"
export  LDFLAGS="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"

make clean
make
make test
make examples