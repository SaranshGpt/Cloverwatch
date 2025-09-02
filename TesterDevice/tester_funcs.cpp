//
// Created by saransh on 8/12/25.
//


#include <iostream>
#include <vector>
#include <cstddef>

#include "libs/crc.h"
#include "libs/ssf_ecc/ssfrs.h"

#include "../data_structures/include/c_types.h"

#include "tester_funcs.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

std::vector<Byte> create_packet_simple(std::vector<Byte> payload, ValidatorConfig config) {
    std::vector<Byte> wrapped_payload;
    wrapped_payload.reserve(config.header_size + config.length_size + payload.size() + config.footer_size);

    for (int i = 0; i < config.header_size; i++) {
        wrapped_payload.push_back(config.header_byte);
    }

    uint64_t payload_len = payload.size();

    if (config.endianness == Endianness::LITTLE) {
        for (int i = 0; i < config.length_size; i++) {
            wrapped_payload.push_back(payload_len & 0xFF);
            payload_len >>= 8;
        }
    } else {
        for (int i = 0; i < config.length_size; i++) {
            wrapped_payload.push_back((payload_len >> (8 * (config.length_size - i - 1))) & 0xFF);
        }
    }

    wrapped_payload.insert(wrapped_payload.end(), payload.begin(), payload.end());

    for (int i = 0; i < config.footer_size; i++) {
        wrapped_payload.push_back(config.footer_byte);
    }

    return wrapped_payload;
}

std::vector<Byte> apply_reed_solomon(std::vector<Byte> &payload, const uint8_t chunk_size, const uint8_t num_symbols) {

    size_t crc_size = 4;
    size_t payload_size = payload.size() + crc_size;

    const auto num_chunks = payload_size / chunk_size + ((payload_size % chunk_size) > 0);

    size_t core_size = num_chunks * chunk_size;
    size_t data_size = core_size - crc_size;
    size_t total_size = core_size + num_symbols * num_chunks;

    payload.resize(total_size, 0);

    for (size_t i = data_size; i < core_size; i++) payload[i] = 0;

    for (size_t i=0; i<data_size; i++) {
        payload[i%crc_size + data_size] ^= payload[i];
    }

    const uint8_t *payload_ptr = payload.data();
    uint8_t *ecc_ptr = &payload[core_size];

    uint16_t buf_len = 0;

    SSFRSEncode(payload_ptr, core_size, ecc_ptr, payload.size() - core_size, &buf_len,
                num_symbols, chunk_size);

    return payload;
}

std::vector<Byte> extract_decoded_payload(std::vector<Byte> &packet, const ValidatorConfig config, const uint8_t chunk_size, const uint8_t num_symbols) {

    const auto payload_start = &(packet[config.header_size + config.length_size]);
    const auto length = packet.size() - config.header_size - config.length_size - config.footer_size;

    uint16_t new_length = 0;

    const auto success = SSFRSDecode(payload_start, length, &new_length, num_symbols, chunk_size);

    return success?
        std::vector<Byte>(payload_start, payload_start + new_length) :
        std::vector<Byte>();
}

PYBIND11_MODULE(TesterLib, m) {
    m.doc() = "pybind11 tester_funcs plugin"; // optional module docstring

    m.def("create_packet_simple", &create_packet_simple, "Create a packet with a simple header and footer");
    m.def("apply_reed_solomon", &apply_reed_solomon, "Apply Reed-Solomon to a payload");
    m.def("extract_decoded_payload", &extract_decoded_payload, "Extract the decoded payload from a packet");

    pybind11::enum_<Endianness>(m, "Endianness")
            .value("LITTLE", Endianness::LITTLE)
            .value("BIG", Endianness::BIG);

    pybind11::class_<ValidatorConfig>(m, "ValidatorConfig")
            .def(pybind11::init<>())
            .def_readwrite("header_byte", &ValidatorConfig::header_byte)
            .def_readwrite("header_size", &ValidatorConfig::header_size)
            .def_readwrite("footer_byte", &ValidatorConfig::footer_byte)
            .def_readwrite("footer_size", &ValidatorConfig::footer_size)
            .def_readwrite("escape_byte", &ValidatorConfig::escape_byte)
            .def_readwrite("length_size", &ValidatorConfig::length_size)
            .def_readwrite("endianness", &ValidatorConfig::endianness);
}
