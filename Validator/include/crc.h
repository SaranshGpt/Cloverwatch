//
// Created by saransh on 8/4/25.
//

#ifndef CRC_H
#define CRC_H

#include <cstdint>
#include <zephyr/sys/util.h>

#include "../../data_structures/include/ptr_types.h"
#include "../../data_structures/include/c_types.h"

namespace Cloverwatch {

    template <typename T>
    T calculate_crc(ReadVector<Byte> buffer) {

        T val = 0;

        for (int i = 0; i < buffer.len; i+=sizeof(T)) {
            auto ptr = buffer.ptr.ptr + i;

            const T* cast_ptr = reinterpret_cast<const T*>(ptr);
            val ^= *cast_ptr;
        }

        auto div = buffer.len / sizeof(T);

        if (div != 0) {
            T curr_val = 0;
            for (int i = 0; i < div; i++) {
                curr_val = (curr_val << 8) | buffer.ptr.ptr[buffer.len - 1 - i];
            }
            val ^= curr_val;
        }

        return val;
    }

    template <typename T>
    bool validate_crc(ReadVector<Byte> buffer) {

        auto ptr = buffer.ptr.ptr + buffer.len - sizeof(T);

        T crc_val;
        bytecpy(&crc_val, ptr, sizeof(T));

        return crc_val == calculate_crc<T>(
            ReadVector<Byte>(buffer.ptr.ptr, buffer.capacity, buffer.len - sizeof(T))
        );
    }

    template <typename T>
    void fill_crc(WriteVector<Byte> buffer) {

        T crc = calculate_crc<T>(ReadVector<Byte>(buffer));

        auto ptr = buffer.ptr.ptr + buffer.len;
        T* cast_ptr = reinterpret_cast<T*>(ptr);
        *cast_ptr = crc;
        bytecpy(buffer.ptr.ptr + buffer.len, &crc, sizeof(T));

        buffer.len += sizeof(T);
    }

}

#endif //CRC_H
