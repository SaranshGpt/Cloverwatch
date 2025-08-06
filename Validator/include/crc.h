//
// Created by saransh on 8/4/25.
//

#ifndef CRC_H
#define CRC_H

#include <cstdint>

#include "../../data_structures/include/ptr_types.h"

namespace Cloverwatch {

    template <typename T>
    T calculate_crc(ReadVector<uint8_t> buffer) {

        T val = 0;

        for (int i = 0; i < buffer.len; i+=sizeof(T)) {
            auto ptr = buffer.ptr.ptr + i;

            const T* cast_ptr = reinterpret_cast<const T*>(ptr);
            val ^= *cast_ptr;
        }

        return val;
    }

    template <typename T>
    bool validate_crc(ReadVector<uint8_t> buffer) {

        auto ptr = buffer.ptr.ptr + buffer.len - sizeof(T);
        const T* cast_ptr = reinterpret_cast<const T*>(ptr);
        return *cast_ptr == calculate_crc<T>(
            ReadVector<uint8_t>(buffer.ptr.ptr, buffer.capacity, buffer.len - sizeof(T))
        );
    }

    template <typename T>
    void fill_crc(WriteVector<uint8_t> buffer) {

        T crc = calculate_crc<T>(ReadVector<uint8_t>(buffer));

        auto ptr = buffer.ptr.ptr + buffer.len;
        T* cast_ptr = reinterpret_cast<T*>(ptr);
        *cast_ptr = crc;

        buffer.len += sizeof(T);
    }

}

#endif //CRC_H
