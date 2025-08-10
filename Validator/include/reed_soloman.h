//
// Created by saransh on 8/4/25.
//

#ifndef REED_SOLOMAN_H
#define REED_SOLOMAN_H

#include <array>

#include "crc.h"
#include "../../data_structures/include/ptr_types.h"

#include "ssfrs.h"
#include "../../data_structures/include/c_types.h"

namespace Cloverwatch {

    namespace RS_Validator {

        template <uint16_t chunk_size, uint8_t max_symbols>
        bool decode(WriteVector<Byte> message_rx) {
            uint16_t rx_len = message_rx.len;
            bool decode_res = SSFRSDecode(reinterpret_cast<uint8_t*>(message_rx.data()), message_rx.len, &rx_len, max_symbols, chunk_size);
            message_rx.len = rx_len;

            return decode_res && validate_crc<uint32_t>(
                ReadVector<Byte>(message_rx.data(), message_rx.capacity, rx_len)
            );
        }
    }

}

#endif //REED_SOLOMAN_H
