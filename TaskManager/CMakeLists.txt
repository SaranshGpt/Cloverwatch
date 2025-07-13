add_library(TaskManager)

find_package(Zephyr)

target_include_directories(
        TaskManager
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(
        TaskManager PRIVATE
        src/task_manager.cpp
)

target_link_libraries(
        TaskManager PUBLIC zephyr_interface
)