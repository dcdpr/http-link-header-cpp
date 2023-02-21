include(CTest)

find_package(doctest REQUIRED)

set(TEST_ENV_VARS "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}")
if (APPLE)
    set(TEST_ENV_VARS "${TEST_ENV_VARS};DYLD_FALLBACK_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")
endif()

add_executable(tests)
target_sources(tests
    PRIVATE
    dev_tests.cpp
    rfc_tests.cpp
    readme_tests.cpp)
target_include_directories(tests
    PRIVATE
    # $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_features(tests PRIVATE cxx_std_17)
target_link_libraries(tests
    PRIVATE
    ${PROJECT_NAME}
    doctest::doctest)

add_test(NAME tests COMMAND tests)
set_tests_properties(tests
    PROPERTIES
    ENVIRONMENT "${TEST_ENV_VARS}")