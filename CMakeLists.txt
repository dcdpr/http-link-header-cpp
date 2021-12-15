cmake_minimum_required(VERSION 3.1)

project(
        "http-link-header-cpp"
        VERSION 0.9.0
        DESCRIPTION
        "A header only c++ library that can parse HTTP Link: header fields as described in RFC8288."
        HOMEPAGE_URL "https://github.com/dcdpr/http-link-header-cpp"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(uriparser 0.9.5 CONFIG REQUIRED char wchar_t)

add_library(${PROJECT_NAME} INTERFACE)
# add alias so the project can be embedded by others and used with add_subdirectory()
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

include(GNUInstallDirs)
include(CTest)

# determine if we are building http-link-header-cpp itself, or if we are embedded in another project
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(IS_TOPLEVEL_PROJECT TRUE)
else()
  set(IS_TOPLEVEL_PROJECT FALSE)
endif()

# only enable installing if we aren't embedded in another project
option(HLH_INSTALL_LIBRARY
        "Enable installing of http-link-header-cpp library into default locations"
        ${IS_TOPLEVEL_PROJECT})

# only enable tests if we aren't embedded in another project
option(HLH_BUILD_TESTING
        "Enable building and running http-link-header-cpp tests"
        ${IS_TOPLEVEL_PROJECT})

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

  install(FILES ${PROJECT_SOURCE_DIR}/include/http-link-header.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()


