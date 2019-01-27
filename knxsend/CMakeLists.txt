set(TARGET_NAME KnxSend)

file(GLOB_RECURSE TARGET_SOURCES
    *.cpp
    ${CMAKE_SOURCE_DIR}/common/*.cpp
)
file(GLOB_RECURSE TARGET_HEADERS
    ${CMAKE_SOURCE_DIR}/lib/*.h
    ${CMAKE_SOURCE_DIR}/common/*.h
    *.h
)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
    ${Boost_INCLUDE_DIR}
    ${LIBSERIAL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(${TARGET_NAME}
    ${Boost_LIBRARIES}
    ${LIBSERIAL_LIBRARIES}
    LibKnxTinySerial
)

target_compile_options(${TARGET_NAME}
  PUBLIC
    ${LIBSERIAL_CFLAGS_OTHER}
)

install (TARGETS ${TARGET_NAME} DESTINATION bin)
