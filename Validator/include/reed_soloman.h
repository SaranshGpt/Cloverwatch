//
// Created by saransh on 8/4/25.
//

#ifndef REED_SOLOMAN_H
#define REED_SOLOMAN_H

#include <array>

#include "crc.h"
#include "../../data_structures/include/ptr_types.h"

#include "ssfrs.h"

namespace Cloverwatch {

    enum class RSResult {
        SUCCESS = 0,
        DECODE_FAIL,
        CRC_FAIL
    };

    template <uint16_t chunk_size, uint8_t max_symbols>
    class RS_Validator {

    public:

        bool decode(WriteVector<Byte> message_rx) {
            int rx_len = message_rx.len;
            bool decode_res = SSFRSDecode(message_rx.ptr.ptr, rx_len, &message_rx.len, max_symbols, chunk_size);
            return decode_res && validate_crc<uint32_t>(message_rx);
        }

    };

}

#endif //REED_SOLOMAN_H
