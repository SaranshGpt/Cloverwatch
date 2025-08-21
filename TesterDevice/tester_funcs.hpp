//
// Created by saransh on 8/21/25.
//

#ifndef TESTER_FUNCS_H
#define TESTER_FUNCS_H

#include <vector>
#include <cstdint>

using Byte = uint8_t;

enum class Endianness {
    LITTLE,
    BIG
};

class ValidatorConfig {

public:
    Byte header_byte;
    uint8_t header_size;

    Byte footer_byte;
    uint8_t footer_size;

    Byte escape_byte;

    uint8_t length_size;

    Endianness endianness;

    ValidatorConfig (Byte header_byte, uint8_t header_size, Byte footer_byte, uint8_t footer_size, Byte escape_byte, uint8_t length_size, Endianness endianness) :
        header_byte(header_byte),
        header_size(header_size),
        footer_byte(footer_byte),
        footer_size(footer_size),
        escape_byte(escape_byte),
        length_size(length_size),
        endianness(endianness)
    {}

    ValidatorConfig() {
        header_byte = 0x00;
        header_size = 0;
        footer_byte = 0x00;
        footer_size = 0;
        escape_byte = 0x00;
        length_size = 0;
        endianness = Endianness::LITTLE;
    }
};

std::vector<Byte> create_packet_simple(std::vector<Byte> payload, ValidatorConfig config);

#endif //TESTER_FUNCS_H
