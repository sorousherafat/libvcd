cmake_minimum_required(VERSION 3.25)
project(libvcd C)

set(CMAKE_C_STANDARD 11)

add_library(libvcd libvcd.c test/test.c)
