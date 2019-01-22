set(TARGET_NAME LibKnxTinySerial)

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
