cmake_minimum_required(VERSION 3.19)
project(http_link_header)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(CXX_FLAGS -Wall -Wextra -Wpedantic -Wno-unknown-pragmas ${CXX_FLAGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(CXX_FLAGS -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded ${CXX_FLAGS})
endif()



find_package(uriparser 0.9.5 CONFIG REQUIRED char wchar_t)

add_library(http_link_header INTERFACE http_link_header.h)

add_executable(tests tests.cpp http_link_header.h)
target_link_libraries(tests PUBLIC uriparser::uriparser)
target_compile_options(tests PRIVATE ${CXX_FLAGS} -Wno-zero-as-null-pointer-constant -Wno-exit-time-destructors -Wno-global-constructors)

