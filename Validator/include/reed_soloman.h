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

    template <uint8_t chunk_size, uint8_t max_symbols>
    class RS_Validator {
    public:
        static bool decode(WriteVector<Byte> message_rx_ref) {

            auto& message_rx = message_rx_ref.ref;

            uint16_t rx_len = message_rx.len();
            bool decode_res = SSFRSDecode(message_rx.data(), message_rx.len(), &rx_len, max_symbols, chunk_size);
            message_rx.set_len(rx_len);

            return decode_res && validate_crc(ToRead(message_rx), 4);
        }

        static bool encode(WriteVector<Byte> message_buffer_ref) {

            auto& message_buffer = message_buffer_ref.ref;

            if (message_buffer.capacity() < message_buffer.len() + max_symbols * chunk_size) {
                return false;
            }

            uint16_t ecc_buf_len;

            SSFRSEncode(
                message_buffer.data(),
                message_buffer.len(),
                message_buffer.data() + message_buffer.len(),
                message_buffer.capacity() - message_buffer.len(),
                &ecc_buf_len,
                max_symbols,
                chunk_size
                );

            message_buffer.set_len( message_buffer.len() + ecc_buf_len);

            return true;
        }

    };

}

#endif //REED_SOLOMAN_H
