cmake_minimum_required(VERSION 3.24)

project(
    "http-link-header-cpp"
    VERSION 0.9.0
    DESCRIPTION
    "A header only c++ library that can parse HTTP Link: header fields as described in RFC8288."
    HOMEPAGE_URL "https://github.com/dcdpr/http-link-header-cpp"
    LANGUAGES CXX)

option(HLH_BUILD_TESTING  "Enable unit tests" ON)
option(USE_CONAN "If Conan is installed, use Conan to pull the project dependencies" OFF)

###############
# Conan support
###############

find_program (CONAN_BIN conan)
if(CONAN_BIN AND USE_CONAN)
    message (STATUS "Found conan C++ package manager: ${CONAN_BIN}")
    find_file (CONANFILE NAMES "conanfile.txt" HINTS "${CMAKE_SOURCE_DIR}")
    if (CONANFILE)
        message (STATUS "Found ${CONANFILE}")
        if (NOT EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
            message(STATUS "Trying to execute 'conan install'")
            get_filename_component(CONANFILE_DIR ${CONANFILE} DIRECTORY)
            execute_process(COMMAND conan install ${CONANFILE_DIR} --build=missing)
        endif()
    endif()

    if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        message(STATUS "conan_basic_setup()")
        conan_basic_setup(TARGETS)
    else()
        message(WARNING "Please run 'conan install' if you plan to use conan")
    endif()
endif()

##########################################
# Local CMake scripts and finders (if any)
##########################################

if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
endif()

###############################
# Check compiler's capabilities
###############################

include (CheckCCompilerFlag)
include (CheckCXXCompilerFlag)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set (CMAKE_COMPILER_IS_CLANG true)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set (CMAKE_COMPILER_IS_MSVC true)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    find_program (LINKER_BIN gold)
    if(LINKER_BIN)
        set(LINKER_BIN "gold")
    else()
        find_program (LINKER_BIN ld.gold)
        if(LINKER_BIN)
            set(LINKER_BIN "ld.gold")
        else()
            set(LINKER_BIN "ld")
        endif()
    endif()

    if (CMAKE_COMPILER_IS_GNUCXX)
        add_compile_options(-fdiagnostics-color=always)
    elseif (CMAKE_COMPILER_IS_CLANG)
        add_compile_options(-fcolor-diagnostics)
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    #set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address")

    CHECK_CXX_COMPILER_FLAG("-fuse-ld=${LINKER_BIN}" USE_LINKER_LD_GOLD)
    if(USE_LINKER_LD_GOLD)
        set (CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=${LINKER_BIN} ${CMAKE_SHARED_LINKER_FLAGS}")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH OFF)
endif()

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
set (CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
set (CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
message(STATUS "CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")


###############################
# Library
###############################

find_package(uriparser 0.9.7 CONFIG REQUIRED char wchar_t)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_link_libraries(${PROJECT_NAME} INTERFACE uriparser::uriparser)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/http-link-header" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS ${PROJECT_NAME}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

###############################
# Testing
###############################

if(HLH_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
else()
    message(STATUS "Skipping unit tests")
endif()