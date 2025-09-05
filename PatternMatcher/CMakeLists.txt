add_library(PatternMatcher)

target_include_directories(
        PatternMatcher
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(
        PatternMatcher PRIVATE
        src/regex.cpp
)