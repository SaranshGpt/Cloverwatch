//
// Created by saransh on 8/4/25.
//

#ifndef CRC_H
#define CRC_H

#include <cstdint>

#ifndef VALIDATOR_LOG_SKIP
#include <zephyr/sys/util.h>
#include "Logger/include/logger.h"
#else
#define bytecpy(dst, src, len) memcpy(dst, src, len)
#endif

#include "../../data_structures/include/ptr_types.h"
#include "../../data_structures/include/c_types.h"

namespace Cloverwatch {

    constexpr int max_crc_size = 8;

    static void calculate_crc(ReadVector<Byte> buffer, WriteVector<Byte> crc) {

        const size_t num_bytes = crc.len;

        for (size_t i = 0; i < buffer.len; i++)
            crc[i%num_bytes] ^= buffer[i];
    }

    bool validate_crc(ReadVector<Byte> buffer, size_t num_bytes) {

        Byte crc_fill[max_crc_size];
        size_t len = num_bytes;

        for (size_t i = 0; i < len; i++) crc_fill[i] = buffer[buffer.len - num_bytes + i];

        size_t temp_len = buffer.len - num_bytes;

        calculate_crc(
            ReadVector<Byte>(buffer.ptr, buffer.capacity, temp_len),
            WriteVector<Byte>(crc_fill, max_crc_size, len)
            );

        for (size_t i = 0; i < len; i++)
            if (crc_fill[i]) return false;

        return true;
    }

    void fill_crc(WriteVector<Byte> buffer, size_t num_bytes) {

        size_t temp_len = buffer.len - num_bytes;

        for (size_t i=0; i < num_bytes; i++)
            buffer[buffer.len - num_bytes + i] = 0;

        calculate_crc(
            ReadVector<Byte>(buffer.ptr.ptr, buffer.capacity, temp_len),
            WriteVector<Byte>(buffer.ptr.ptr + temp_len, buffer.capacity - temp_len, num_bytes)
        );

    }

}

#endif //CRC_H
