cmake_minimum_required(VERSION 3.19)
project(http_link_header)

set(CMAKE_CXX_STANDARD 11)

find_package(uriparser 0.9.5 CONFIG REQUIRED char wchar_t)

add_library(http_link_header INTERFACE http_link_header.h)

add_executable(tests tests.cpp http_link_header.h)
target_link_libraries(tests PUBLIC uriparser::uriparser)

