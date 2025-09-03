//
// Created by saransh on 8/4/25.
//

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "reed_soloman.h"
#include "../../data_structures/include/c_types.h"

namespace Cloverwatch {

	enum class Endianness {
		LITTLE,
		BIG
	};

    struct ValidatorConfig {
        Byte header_byte;
        uint8_t header_size;

        Byte footer_byte;
        uint8_t footer_size;

        Byte escape_byte;

        uint8_t length_size;

    	Endianness endianness;
    };

	using BlockEccFunc = bool (*)(WriteVector<Byte> message_rx);

	//TODO: Implement Header CRC for Block Validator.  Right now there is nothing handling erros in header values. Skipping this now as the header is only length as of now

    template <uint16_t buffer_size, BlockEccFunc encode_func, BlockEccFunc decode_func>
    class SimplePacketiser_Block {
	public:

		void add_bytes(ReadVector<Byte> message_rx, WriteVector<Byte> message_tx);
    	void set_validator_config(ValidatorConfig config);

    	void reset();

    	void construct_packet(ReadVector<Byte> payload, WriteVector<Byte> packet) const;

    	constexpr explicit SimplePacketiser_Block(ValidatorConfig config) : config(config) {}

	private:

    	ValidatorConfig config;

		enum class State {
			HEADER,
			LENGTH,
			PAYLOAD,
			FOOTER
		} curr_state = State::HEADER;

		size_t bytes_since_last_state_change = 0;
    	bool escape_expected = false;

    	uint64_t expected_length = 0;

		Buffer<buffer_size> buffer;

    	void append_length_byte(Byte byte);

    };

}

#include "../src/simple_packetiser.tpp"

#endif //VALIDATOR_H
