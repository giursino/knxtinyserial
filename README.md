# knxtinyserial

This project enable to access to KNX bus with the "KNX Tiny Interface 810" by Weinzierl Engineering GmbH. 
datasheet https://www.weinzierl.de/index.php/en/all-knx/knx-module-en/knx-tinyserial-interface-810-en

This project does not use the Weinzierl SDK and it is test to Raspberry Pi 1 with GCC 6.3.

It is written with C++11.

This project generate a library that can be used on other projects and generate two simple program 
to send and read to/from KNX bus.

## Build for Raspberry

### Prerequisites
* C++11 (GCC 6.3)
* CMAKE >= 2.8
* pkgconfig
* libserial-dev (>= 0.6.0~rc2+svn122-4)
* libboost-filesystem1.62-dev
* libboost-program-options1.62-dev
 
### Configure, build and install
```
git clone git@github.com:giursino/knxtinyserial.git
mkdir build
cd build
cmake -D CMAKE_TOOLCHAIN_FILE=../CMakeToolchainFile-RaspberryGCC6.cmake  ..
make
DEST=/tmp
make DESTDIR=${DEST} install
```

### Build distribution
```
# build a binary distribution
cpack --config CPackConfig.cmake

# build a source distribution
cpack --config CPackSourceConfig.cmake
```

## Use
* `knxread` to read from KNX bus
* `knxsend 11223344556677889900AABBCCDDEEFF` to send to KNX bus
