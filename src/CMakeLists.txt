set(TARGET_NAME LibSerialTest)

file(GLOB_RECURSE TARGET_SOURCES *.cpp)
file(GLOB_RECURSE TARGET_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
)

if(ENABLE_SERIAL)
    target_include_directories(${TARGET_NAME}
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${LIBSERIAL_INCLUDE_DIRS}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(${TARGET_NAME}
      ${LIBSERIAL_LIBRARIES}
      pthread
    )

    target_compile_options(${TARGET_NAME}
    PUBLIC
      ${LIBSERIAL_CFLAGS_OTHER}
    )
else()
    target_include_directories(${TARGET_NAME}
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include
    )
endif()
