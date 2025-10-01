add_library(Cli)

find_package(Zephyr)

target_include_directories(
        Cli
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(
        Cli PRIVATE
        src/cli.cpp
)

target_link_libraries(
        Cli PUBLIC zephyr_interface
)