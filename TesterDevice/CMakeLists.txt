cmake_minimum_required(VERSION 3.20)

project(TesterDeviceLibs)

file(GLOB_RECURSE tester_libsrc
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/*c"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/*cpp"
)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message("TesterDevice: Fount Python Toolchain")
find_package(pybind11 REQUIRED)
message("TesterDevice: Found pybind11")

pybind11_add_module(TesterLib
        ${tester_libsrc}
        tester_funcs.cpp
)

target_compile_features(TesterLib PRIVATE cxx_std_17)

target_include_directories(TesterLib PRIVATE
        libs/ssf_ecc
        libs
)

target_compile_definitions(TesterLib PRIVATE VALIDATOR_LOG_SKIP)

add_custom_command(TARGET TesterLib POST_BUILD
        COMMAND PYTHONPATH=./cmake-build-debug/ ${CMAKE_CURRENT_SOURCE_DIR}/.venv/bin/pybind11-stubgen TesterLib --output-dir=${CMAKE_CURRENT_SOURCE_DIR}/device/
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying stub file to build directory"
)

message("TesterDevice: TesterLib initialised successfully")