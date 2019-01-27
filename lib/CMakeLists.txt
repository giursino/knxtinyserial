set(TARGET_NAME KnxTinySerial)

file(GLOB_RECURSE TARGET_SOURCES
    *.cpp
    ${CMAKE_SOURCE_DIR}/common/*.cpp
)
file(GLOB_RECURSE TARGET_HEADERS
    ${CMAKE_SOURCE_DIR}/common/*.h
    *.h
)

add_library(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE
    ${CMAKE_SOURCE_DIR}/common
)

install (TARGETS ${TARGET_NAME} DESTINATION lib)
install (FILES ${TARGET_HEADERS} DESTINATION include)

# generate pkg-file
configure_file(${TARGET_NAME}.pc.in ${TARGET_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)
