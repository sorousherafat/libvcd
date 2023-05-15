#!/usr/bin/env sh

cp cmake-build-debug/liblibvcd.a test/libvcd.o || exit
gcc -g -Wall test/test.c test/libvcd.o -o test/test.out || exit
test/test.out $*
rm test/test.out
