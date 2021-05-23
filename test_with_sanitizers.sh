#!/bin/sh

export CC=clang 
export CXX=clang++
export  CFLAGS="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"
export  CXXFLAGS="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"
export  LDFLAGS="-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=undefined"

make clean
make
make test