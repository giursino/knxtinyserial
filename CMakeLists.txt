cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(LibSerialTestProject)

# enable support for c++11
set (CMAKE_CXX_STANDARD 11)

# add Test option
option(ENABLE_SERIAL "Enable serial" OFF)

# check pkg-config support
find_package(PkgConfig REQUIRED)

if(ENABLE_SERIAL)
    add_definitions(-DENABLE_SERIAL)

    # check required libraries
    pkg_check_modules(LIBSERIAL REQUIRED libserial)

    link_directories(
        ${LIBSERIAL_LIBRARY_DIRS}
    )

    include_directories(
        ${LIBSERIAL_INCLUDE_DIRS}
    )
endif(ENABLE_SERIAL)

add_subdirectory(lib)
add_subdirectory(src)
