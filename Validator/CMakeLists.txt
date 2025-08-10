add_library(Validator)

target_include_directories(
    Validator
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/libcorrect/include"

        "${CMAKE_CURRENT_SOURCE_DIR}/libs/ssf_ecc"
)

file(GLOB rs_sources "${CMAKE_CURRENT_SOURCE_DIR}/libs/ssf_ecc/*.c")

target_sources(
        Validator
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/reed_soloman.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/viterbi.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/validator.cpp"
        ${rs_sources}
)

target_link_libraries(
        Validator PRIVATE
        Logger
        zephyr_interface
)