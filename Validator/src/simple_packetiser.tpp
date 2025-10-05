//
// Created by saransh on 8/6/25.
//

#ifndef VALIDATOR_TPP
#define VALIDATOR_TPP


#include "simple_packetiser.h"

#ifndef VALIDATOR_LOG_SKIP
#include <logger.h>
#endif

#include "../../system_config.h"

#include "../../data_structures/include/ptr_types.h"

namespace Cloverwatch {

    static void log_error(const char *msg) {
#ifndef VALIDATOR_LOG_SKIP
        Logger<ModuleId::VALIDATION_MODULE>::log(msg, LogLevel::ERROR);
#endif
    };

    template <typename G, typename L>
    static inline bool is_special(const Byte &byte, const ValidatorConfig &config) {
        return
            (byte == L::escape_byte) ||
            (byte == L::header_byte) ||
            (byte == L::footer_byte);
    }

    template <typename G, typename L,  BlockEccFunc encode_func, BlockEccFunc decode_func>
    void SimplePacketiser_Block<G, L, encode_func, decode_func>::reset() {
        curr_state = State::HEADER;
        bytes_since_last_state_change = 0;
        escape_expected = false;
        expected_length = 0;
        buffer.clear();
    }

    template <typename G, typename L,  BlockEccFunc encode_func, BlockEccFunc decode_func>
    void SimplePacketiser_Block<G, L, encode_func, decode_func>::append_length_byte(Byte byte) {

        switch (L::endianness) {
            case ValidatorConfig::Endianness::LITTLE: {
                expected_length |= static_cast<uint64_t>(byte) << (8 * bytes_since_last_state_change);
                return;
            }
            case ValidatorConfig::Endianness::BIG: {
                expected_length |= static_cast<uint64_t>(byte) << (8 * (L::length_size - bytes_since_last_state_change - 1));
                return;
            }
        }

    }

    template <typename G, typename L,  BlockEccFunc encode_func, BlockEccFunc decode_func>
    void SimplePacketiser_Block<G, L, encode_func, decode_func>::add_byte(Byte byte, WriteVector<Byte> message_tx) {

        auto escape_needed = [this](Byte byte) {
            return (curr_state != State::HEADER && curr_state != State::FOOTER) && (byte == L::header_byte || byte == L::footer_byte || byte == L::escape_byte);
        };

        auto set_state = [this](State state) {
            curr_state = state;
            bytes_since_last_state_change = 0;
        };

        if (escape_expected) {
            if (byte == L::escape_byte) {
                escape_expected = false;
            } else {
                reset();
            }
            return;
        }

        escape_expected = escape_needed(byte);

        switch (curr_state) {
            case State::HEADER: {
                if (byte != L::header_byte) {
                    reset();
                    return;
                }

                bytes_since_last_state_change++;

                if (bytes_since_last_state_change == L::header_size)
                    set_state(State::LENGTH);

                break;
            }
            case State::LENGTH: {
                append_length_byte(byte);
                bytes_since_last_state_change++;
                if (bytes_since_last_state_change == L::length_size) {
                    if (expected_length > G::max_packet_size) {
                        log_error("Expected length exceeded buffer capacity. Discarding packet");
                        reset();
                        return;
                    } else {
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
                if (byte != L::footer_byte) {
                    reset();
                    return;
                }

                bytes_since_last_state_change++;

                if (bytes_since_last_state_change == L::footer_size) {
                    bool valid;

                    if constexpr (decode_func != nullptr)
                        valid = decode_func(ToWrite(buffer.as_vec()));
                    else
                        valid = true;

                    if (valid) {
                        for (size_t i = 0; i < buffer.len(); i++) {
                            message_tx.ref[i] = buffer[i];
                        }
                        message_tx.ref.set_len(buffer.len());
                    } else {
                        log_error("Validation failed. Discarding packet");
                    }

                    reset();
                    break;
                }
            }
        };
    }

    template<typename G, typename L, BlockEccFunc encode_func, BlockEccFunc decode_func>
    void SimplePacketiser_Block<G, L, encode_func, decode_func>::construct_packet(
        ReadVector<Byte> payload, WriteVector<Byte> packet) const {
        for (size_t i = 0; i < L::header_size; i++)
            packet.ref.push_back(L::header_byte);

        size_t payload_len = payload.ref.len();

        switch (L::endianness) {
            case ValidatorConfig::Endianness::BIG: {
                for (size_t i=0; i<L::length_size; i++)
                    packet.ref.push_back(static_cast<Byte>(payload_len >> (8 * (L::length_size - i - 1))));
                break;
            }
            case ValidatorConfig::Endianness::LITTLE: {
                for (size_t i=0; i<L::length_size; i++)
                    packet.ref.push_back(static_cast<Byte>(payload_len >> (8 * i)));
                break;
            }
            default:
                log_error("Unexpected endianness value encountered");
        }

        for (size_t i=0; i<payload.ref.len(); i++)
            packet.ref.push_back(payload.ref[i]);

        size_t temp_len = payload.ref.len();



        encode_func(packet);

        packet.ref.set_len(temp_len + L::header_size + L::length_size);

        for (size_t i=0; i<L::footer_size; i++)
            packet.ref.push_back(L::footer_byte);

    }

}

#endif //VALIDATOR_TPP
