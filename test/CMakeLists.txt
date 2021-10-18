cmake_minimum_required(VERSION 3.1)

add_executable(tests)
target_sources(
        tests
        PRIVATE tests.cpp)
target_include_directories(
        tests PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_compile_features(tests PRIVATE cxx_std_11)
target_link_libraries(tests PUBLIC http_link_header::http_link_header)

target_compile_options(tests PRIVATE ${CXX_FLAGS} -Wno-zero-as-null-pointer-constant -Wno-exit-time-destructors -Wno-global-constructors)

# what the what?
#target_compile_options(
#        SI_detail_tests
#        PRIVATE
#        $<$<CXX_COMPILER_ID:MSVC>:/std:c++17>
#        $<$<CXX_COMPILER_ID:MSVC>:/W4>
#        $<$<CXX_COMPILER_ID:MSVC>:/WX>
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wall>
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Werror>
#        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-pedantic>
#)

add_test(NAME tests COMMAND tests)
