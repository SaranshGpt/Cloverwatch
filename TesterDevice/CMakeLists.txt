cmake_minimum_required(VERSION 3.20)

project(TesterDeviceLibs)

add_library(TesterLib)

file(GLOB_RECURSE libsrc "${CMAKE_CURRENT_SOURCE_DIR}/libs")

include_directories(
        libs/ssf_ecc
)

target_sources(TesterLib
        PRIVATE
        ${libsrc}
        tester_funcs.cpp
)

target_compile_definitions(TesterLib PRIVATE VALIDATOR_LOG_SKIP)