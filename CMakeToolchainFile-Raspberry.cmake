set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv6l)
set(CMAKE_SYSTEM_VERSION 1)
 
# Specify the sdk path
set(SDK_PATH /opt/rpi-sdk/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64)
 
# specify the cross compiler
set(CMAKE_C_COMPILER   "${SDK_PATH}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${SDK_PATH}/bin/arm-linux-gnueabihf-g++")
 
# Specify flags
set(COMMON_FLAGS "-march=armv7-a -marm -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a9")
set(COMMON_FLAGS "")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}" CACHE STRING "")
 
# Specify the system sysroot
set(CMAKE_SYSROOT "/mnt/ggrasp")
 
# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
 
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
 
set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
