add_library(Serial_IO)

find_package(Zephyr)

target_include_directories(
        Serial_IO PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(Serial_IO PRIVATE
    src/serial_io.cpp compile_time_init/serial_template_config.cpp
)

target_link_libraries(
        Serial_IO PRIVATE
        zephyr_interface
        TaskManager
        Logger
)