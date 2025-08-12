//
// Created by saransh on 8/6/25.
//

#ifndef VALIDATOR_TPP
#define VALIDATOR_TPP


#include "simple_packetiser.h"

#ifndef VALIDATOR_LOG_SKIP
#include <logger.h>
#endif

#include "../../data_structures/include/ptr_types.h"

namespace Cloverwatch {

    static void log_error(const char *msg) {
#ifndef VALIDATOR_LOG_SKIP
        Logger<ModuleId::VALIDATION_MODULE>::log(ReadPtr<char>(msg), LogLevel::ERROR);
#endif
    };

    static inline bool is_special(const Byte &byte, const ValidatorConfig &config) {
        return
            (byte == config.escape_byte) ||
            (byte == config.header_byte) ||
            (byte == config.footer_byte);
    }

    template <uint16_t buffer_size, BlockValidationFunc validation_func>
    void SimplePacketiser_Block<buffer_size, validation_func>::set_validator_config(ValidatorConfig config) {
        this->config = config;
    }

    template <uint16_t buffer_size, BlockValidationFunc validation_func>
    void SimplePacketiser_Block<buffer_size, validation_func>::reset() {
        curr_state = State::HEADER;
        bytes_since_last_state_change = 0;
        escape_expected = false;
        expected_length = 0;
        buffer.clear();
    }

    template <uint16_t buffer_size, BlockValidationFunc validation_func>
    void SimplePacketiser_Block<buffer_size, validation_func>::append_length_byte(Byte byte) {

        switch (config.endianness) {
            case Endianness::LITTLE: {
                expected_length |= static_cast<uint64_t>(byte) << (8 * bytes_since_last_state_change);
                return;
            }
            case Endianness::BIG: {
                expected_length |= static_cast<uint64_t>(byte) << (8 * (config.length_size - bytes_since_last_state_change - 1));
                return;
            }
        }

    }

    template <uint16_t buffer_size, BlockValidationFunc validation_func>
    void SimplePacketiser_Block<buffer_size, validation_func>::add_bytes(ReadVector<Byte> message_rx, WriteVector<Byte> message_tx) {

        auto escape_needed = [this](Byte byte) {
            return (curr_state != State::HEADER && curr_state != State::FOOTER) && (byte == config.header_byte || byte == config.footer_byte || byte == config.escape_byte);
        };

        auto set_state = [this](State state) {
            curr_state = state;
            bytes_since_last_state_change = 0;
        };

        for(size_t i = 0; i < message_rx.len; i++) {

            auto byte = message_rx.ptr.ptr[i];

            if (escape_expected) {
                if (byte == config.escape_byte) {
                    escape_expected = false;
                }
                else {
                    reset();
                }
                continue;
            }

            escape_expected = escape_needed(byte);

            switch (curr_state) {
                case State::HEADER: {
                    if (byte != config.header_byte) {
                        reset();
                        continue;
                    }

                    bytes_since_last_state_change++;

                    if (bytes_since_last_state_change == config.header_size)
                        set_state(State::LENGTH);

                    break;
                }
                case State::LENGTH: {
                    append_length_byte(byte);
                    bytes_since_last_state_change++;
                    if (bytes_since_last_state_change == config.length_size) {

                        if (expected_length > buffer_size) {
                            log_error("Expected length exceeded buffer capacity. Discarding packet");
                            reset();
                            continue;
                        }
                        else {
                            set_state(State::PAYLOAD);
                        }
                    }
                    break;
                }
                case State::PAYLOAD: {
                    buffer.push_back(byte);
                    bytes_since_last_state_change++;
                    if (bytes_since_last_state_change == expected_length)
                        set_state(State::FOOTER);
                    break;
                }
                case State::FOOTER: {
                    if (byte != config.footer_byte) {
                        reset();
                        continue;
                    }

                    bytes_since_last_state_change++;

                    if (bytes_since_last_state_change == config.footer_size) {

                        bool valid;

                        if constexpr (validation_func != nullptr)
                            valid = validation_func(buffer.to_WriteVector());
                        else
                            valid = true;

                        if (valid) {
                            for (int i=0; i<buffer.len; i++) {
                                message_tx[i] = buffer[i];
                            }
                            message_tx.len = buffer.len;
                        }
                        else {
                            log_error("Validation failed. Discarding packet");
                        }

                        reset();
                        break;
                    }

                }

            };

        }

    }

}

#endif //VALIDATOR_TPP
