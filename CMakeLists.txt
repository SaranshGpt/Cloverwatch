cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Cloverwatch)

add_subdirectory(Serial_IO)
add_subdirectory(TaskManager)
add_subdirectory(Logger)
add_subdirectory(Validator)
add_subdirectory(PatternMatcher)
add_subdirectory(CLI)

target_sources(app PRIVATE main.cpp Config/object_init.cpp)
target_link_libraries(app PRIVATE
        Serial_IO TaskManager Logger Validator PatternMatcher Cli
)

add_compile_options(-fwrapv)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/libs/ssf_ecc"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Validator/libs/ssf_ecc"
        "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/libs/ssf_ecc"

        COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/libs/crc.h"
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/Validator/include/crc.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/libs/crc.h"
)

include(ExternalProject)
ExternalProject_Add(TesterDevice
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice"
        BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TesterDevice/cmake-build-debug"
        CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        BUILD_ALWAYS TRUE
        INSTALL_COMMAND ""
)

add_dependencies(app TesterDevice)