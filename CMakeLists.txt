cmake_minimum_required(VERSION 3.20)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Cloverwatch)

add_subdirectory(Serial_IO)
add_subdirectory(TaskManager)
add_subdirectory(Logger)
add_subdirectory(Validator)

target_sources(app PRIVATE main.cpp)
target_link_libraries(app PRIVATE
    Serial_IO TaskManager Logger
)