cmake_minimum_required(VERSION 3.20)

find_package(Zephyr)
project(Cloverwatch)

add_subdirectory(Serial_IO)
add_subdirectory(data_structures)
add_subdirectory(TaskManager)

if(NOT QEMU_EXTRA_ARGS)
    set(QEMU_EXTRA_ARGS "")
endif()

list(APPEND QEMU_EXTRA_ARGS "-serial")
list(APPEND QEMU_EXTRA_ARGS "pty")
list(APPEND QEMU_EXTRA_ARGS "-icount" "shift=5,align=off")

target_sources(app PRIVATE main.cpp)
target_link_libraries(app PRIVATE
    Serial_IO DataStructures TaskManager
)