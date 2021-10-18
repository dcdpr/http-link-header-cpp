cmake_minimum_required(VERSION 3.1)

project(
        "http_link_header"
        VERSION 1.0.0
        DESCRIPTION
        "A header only c++ library that can parse HTTP Link: header fields as described in RFC8288."
        HOMEPAGE_URL "https://github.com/dcdpr/http_link_header-cpp"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
  set(CXX_FLAGS -Wall -Wextra -Wpedantic -Wno-unknown-pragmas ${CXX_FLAGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(CXX_FLAGS -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded ${CXX_FLAGS})
endif()

find_package(uriparser 0.9.5 CONFIG REQUIRED char wchar_t)

add_library(${PROJECT_NAME} INTERFACE)
# add alias so the project can be uses with add_subdirectory
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)
include(CTest)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_TOPLEVEL_PROJECT TRUE)
else()
  set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(HLH_INSTALL_LIBRARY
        "Enable installing of http_link_header library into default locations"
        ${IS_TOPLEVEL_PROJECT})
option(HLH_BUILD_TESTING "Build and run http_link_header tests " ${IS_TOPLEVEL_PROJECT})

target_include_directories(
        ${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_11)

target_link_libraries(${PROJECT_NAME} INTERFACE uriparser::uriparser)

if(BUILD_TESTING AND HLH_BUILD_TESTING)
  add_subdirectory(test)
endif()


if(HLH_INSTALL_LIBRARY)
  # locations are provided by GNUInstallDirs
  install(
          TARGETS ${PROJECT_NAME}
          EXPORT ${PROJECT_NAME}_Targets
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
          "${PROJECT_NAME}ConfigVersion.cmake"
          VERSION ${PROJECT_VERSION}
          COMPATIBILITY SameMajorVersion)

  configure_package_config_file(
          "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
          INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(
          EXPORT ${PROJECT_NAME}_Targets
          FILE ${PROJECT_NAME}Targets.cmake
          NAMESPACE ${PROJECT_NAME}::
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
          "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
          DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(FILES ${PROJECT_SOURCE_DIR}/include/http_link_header.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()


