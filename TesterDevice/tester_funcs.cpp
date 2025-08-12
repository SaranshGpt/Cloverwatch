//
// Created by saransh on 8/12/25.
//


#include <iostream>
#include <vector>
#include <cstddef>

#include "crc.h"
#include "ssfrs.h"

extern "C" {

    using Byte = uint8_t;

    enum class Endianness {
        LITTLE,
        BIG
    };

    struct ValidatorConfig {
        Byte header_byte;
        uint8_t header_size;

        Byte footer_byte;
        uint8_t footer_size;

        Byte escape_byte;

        uint8_t length_size;

        Endianness endianness;
    };

    std::vector<Byte> create_packet_simple(std::vector<Byte> payload, ValidatorConfig config) {
        std::vector<Byte> wrapped_payload;
        wrapped_payload.reserve(config.header_size + config.length_size + payload.size() + config.footer_size);

        for (int i=0; i<config.header_size; i++) {
            wrapped_payload.push_back(config.header_byte);
        }

        uint64_t payload_len = payload.size();

        if (config.endianness == Endianness::LITTLE) {
            for (int i=0; i<config.length_size; i++) {
                wrapped_payload.push_back(payload_len & 0xFF);
                payload_len >>= 8;
            }
        }
        else {
            for (int i=0; i<config.length_size; i++) {
                wrapped_payload.push_back((payload_len >> (8 * (config.length_size - i - 1))) & 0xFF);
            }
        }

        wrapped_payload.insert(wrapped_payload.end(), payload.begin(), payload.end());

        for (int i=0; i<config.footer_size; i++) {
            wrapped_payload.push_back(config.footer_byte);
        }

        return wrapped_payload;
    }

    void apply_reed_solomon(std::vector<Byte> &payload, const uint8_t chunk_size, const uint8_t num_symbols) {

        const auto num_chunks = (payload.size()+4) / chunk_size + ((payload.size()+4) % chunk_size > 0);

        const auto core_size = num_chunks * chunk_size;
        const auto total_size = core_size + num_symbols * num_chunks;

        payload.reserve(total_size);

        while (payload.size() < core_size)
            payload.push_back(0);

        payload.resize(total_size);

        auto write_vec = Cloverwatch::WriteVector<Byte>(&payload[0], core_size, core_size-4);

        Cloverwatch::fill_crc<uint32_t>(write_vec);

        const uint8_t* payload_ptr = payload.data();
        uint8_t* ecc_ptr = &payload[core_size];

        uint16_t buf_len = 0;

        SSFRSEncode(payload_ptr, num_chunks * chunk_size, ecc_ptr, payload.size() - num_chunks * chunk_size, &buf_len, num_symbols, chunk_size);
    }

}