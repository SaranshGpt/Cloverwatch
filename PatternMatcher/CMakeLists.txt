add_library(PatternMatcher)

find_package(Zephyr)

target_include_directories(
        PatternMatcher
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(
        PatternMatcher PRIVATE
        src/pattern.cpp
        src/pattern_stats.cpp
        compile_time_init/pattern_template_config.cpp
)

target_link_libraries(
        PatternMatcher zephyr_interface TaskManager Logger
)