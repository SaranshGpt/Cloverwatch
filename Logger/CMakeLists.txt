add_library(Logger)

find_package(Zephyr)

target_include_directories(
        Logger PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(
        Logger PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp"
)

target_link_libraries(
        Logger PRIVATE
        zephyr_interface
        TaskManager
)