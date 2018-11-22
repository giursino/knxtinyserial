set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
 
# Specify the sdk path
set(SDK_PATH /home/giursino/Scaricati/rpi-newer-crosstools-master/x64-gcc-6.3.1/arm-rpi-linux-gnueabihf)

# specify the cross compiler
set(CMAKE_C_COMPILER   "${SDK_PATH}/bin/arm-rpi-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${SDK_PATH}/bin/arm-rpi-linux-gnueabihf-g++")

# Specify flags
set(COMMON_FLAGS "-march=armv6z -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}" CACHE STRING "")
 
# Specify the system sysroot
#set(CMAKE_SYSROOT "/home/giursino/Scaricati/sysroot-glibc-linaro-2.23-2017.05-arm-linux-gnueabihf")
 
# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
 
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
 
set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
