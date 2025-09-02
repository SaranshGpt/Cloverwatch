cmake_minimum_required(VERSION 3.20)

add_library(Validator)

set(Validator_Includes
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/libcorrect/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/ssf_ecc"
)

target_include_directories(
    Validator
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../"

        "${CMAKE_CURRENT_SOURCE_DIR}/libs/ssf_ecc"
)

file(GLOB rs_sources "${CMAKE_CURRENT_SOURCE_DIR}/libs/ssf_ecc/*.c")

target_sources(
        Validator
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/reed_soloman.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/viterbi.cpp"
        ${rs_sources}
)

target_link_libraries(
        Validator PRIVATE
        Logger
        zephyr_interface
)