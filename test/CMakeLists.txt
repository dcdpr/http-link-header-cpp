cmake_minimum_required(VERSION 3.1)

add_executable(tests)
target_sources(
        tests
        PRIVATE dev_tests.cpp rfc_tests.cpp)
target_include_directories(
        tests PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_compile_features(tests PRIVATE cxx_std_11)
target_compile_options(
        tests
        PRIVATE ${CXX_FLAGS}
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:GNU>:-Wextra>
        $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>
        $<$<CXX_COMPILER_ID:GNU>:-Wno-unknown-pragmas>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Weverything>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-c++98-compat>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-c++98-compat-pedantic>
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-padded>
        -Wno-zero-as-null-pointer-constant
        -Wno-exit-time-destructors
        -Wno-global-constructors)
target_link_libraries(tests PUBLIC http_link_header::http_link_header)

add_test(NAME tests COMMAND tests)
