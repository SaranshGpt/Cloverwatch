cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Cloverwatch)

add_subdirectory(Serial_IO)
add_subdirectory(TaskManager)
add_subdirectory(Logger)
add_subdirectory(Validator)

target_sources(app PRIVATE main.cpp)
target_link_libraries(app PRIVATE
        Serial_IO TaskManager Logger Validator
)

add_custom_command(
        TARGET app
        POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Validator/libs/ssf_ecc"
            "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/libs/ssf_ecc"

        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/Validator/include/crc.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/libs/crc.h"

        COMMAND ${CMAKE_COMMAND}
            -S "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice"
            -B "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/build"

        COMMAND ${CMAKE_COMMAND}
            --build "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/build"

        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/build"
)