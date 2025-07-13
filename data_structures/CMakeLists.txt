add_library(DataStructures)

find_package(Zephyr)

target_include_directories(
        DataStructures PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(DataStructures PRIVATE
        src/c_queue.cpp
)

target_link_libraries(
        DataStructures PRIVATE
        zephyr_interface
)