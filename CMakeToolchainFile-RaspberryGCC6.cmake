set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv6l)
set(CMAKE_SYSTEM_VERSION 1)
 
# Specify the sdk path
set(SDK_PATH /opt/rpi-newer-crosstools-master/x64-gcc-6.3.1/arm-rpi-linux-gnueabihf)

# specify the cross compiler
set(CMAKE_C_COMPILER   "${SDK_PATH}/bin/arm-rpi-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${SDK_PATH}/bin/arm-rpi-linux-gnueabihf-g++")

# Specify the system sysroot
set(CMAKE_SYSROOT "/mnt/ggrasp")

# Specify flags
# I need to specify -I, -B and -rpath because the toolchain was build for TARGET arm-rpi-linux-gnueabihf
# (see $CMAKE_CXX_COMPILER -dumpmachine) while the sysroot was created (by raspbian) for target
# arm-linux-gnueabihf. So paths of libc and libraries doesn't match.
set(COMMON_FLAGS " \
    -march=armv6z -mtune=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard \
    -I${CMAKE_SYSROOT}/usr/include/arm-linux-gnueabihf \
    -B${CMAKE_SYSROOT}/lib/arm-linux-gnueabihf \
    -B${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf \
    -Wl,-rpath-link,${CMAKE_SYSROOT}/lib/arm-linux-gnueabihf \
    -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf \
    ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}" CACHE STRING "")
 
# where is the target environment
SET(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
 
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
 
set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/local/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/local/share/pkgconfig:${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf/pkgconfig:${CMAKE_SYSROOT}/usr/local/lib/arm-linux-gnueabihf/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})
