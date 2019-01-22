set(TARGET_NAME SerialTest)

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

if(ONLY_SERIAL)
    target_include_directories(${TARGET_NAME}
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/lib
      ${CMAKE_SOURCE_DIR}/common
    )

    target_link_libraries(${TARGET_NAME}
      LibKnxTinySerial
    )

else()

    target_include_directories(${TARGET_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBSERIAL_INCLUDE_DIRS}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/common
        )

    target_link_libraries(${TARGET_NAME}
        ${LIBSERIAL_LIBRARIES}
        pthread
        LibKnxTinySerial
        )

    target_compile_options(${TARGET_NAME}
        PUBLIC
        ${LIBSERIAL_CFLAGS_OTHER}
        )
endif()
