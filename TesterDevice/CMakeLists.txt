cmake_minimum_required(VERSION 3.20)

project(TesterDeviceLibs)

add_library(TesterLib SHARED)

file(GLOB_RECURSE libsrc
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/*c"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/*cpp"
)

include_directories(
        libs/ssf_ecc
        libs
)

target_sources(TesterLib
        PRIVATE
        ${libsrc}
        tester_funcs.cpp
)

target_compile_definitions(TesterLib PRIVATE VALIDATOR_LOG_SKIP)

install(TARGETS TesterLib
        LIBRARY DESTINATION ${INSTALL_LIB_DIR} lib
        RUNTIME DESTINATION ${INSTALL_LIB_DIR} bin
)

message(STATUS "Found lib source files: ${libsrc}")