//
// Created by saransh on 8/6/25.
//

#include "validator.h"
#include "../../data_structures/include/ptr_types.h""

namespace Cloverwatch {

    template <ValidatorConfig config, uint16_t buffer_size, BlockValidationFunc validation_func>
    void BlockValidator<config, buffer_size, validation_func>::add_bytes(ReadVector<Byte> message_rx, WriteVector<Byte> message_tx) {

        for(int i = 0; i < message_rx.len; i++) {

            Byte curr_byte = message_rx[i];

            switch (curr_state) {

            case State::HEADER:



            case State::LENGTH:

            case State::PAYLOAD:

            case State::FOOTER:

            }

        }

    }

}