cmake_minimum_required(VERSION 3.19)
project(http_link_header)

set(CMAKE_CXX_STANDARD 11)

add_library(http_link_header INTERFACE http_link_header.h)

add_executable(tests tests.cpp http_link_header.h)


