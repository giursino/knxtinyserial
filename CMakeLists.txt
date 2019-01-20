cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(LibSerialTestProject)

# enable support for c++11
set (CMAKE_CXX_STANDARD 11)

# add Test option
option(ONLY_SERIAL "Enable only serial compilation. Do not use KnxTinySerial." OFF)

# check pkg-config support
find_package(PkgConfig REQUIRED)

if(ONLY_SERIAL)
    add_definitions(-DONLY_SERIAL)
endif(ONLY_SERIAL)

# check required libraries
pkg_check_modules(LIBSERIAL REQUIRED libserial)

add_subdirectory(lib)
add_subdirectory(knxread)
add_subdirectory(knxsend)
add_subdirectory(serialtest)
