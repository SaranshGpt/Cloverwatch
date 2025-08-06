//
// Created by saransh on 8/4/25.
//

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "reed_soloman.h"

namespace Cloverwatch {

    struct ValidatorConfig {
        uint8_t header_byte;
        uint8_t header_size;

        uint8_t footer_byte;
        uint8_t footer_size;

        uint8_t escape_byte;

        uint8_t length_size;
    };

	using BlockValidationFunc = bool (*)(WriteVector<Byte> message_rx);

    template <uint16_t buffer_size, BlockValidationFunc validation_func>
    class BlockValidator {
	public:

		void add_bytes(ReadVector<Byte> message_rx, WriteVector<Byte> message_tx);
    	void set_validator_config(ValidatorConfig config);

	private:

		enum class State {
			HEADER,
			LENGTH,
			PAYLOAD,
			FOOTER
		} curr_state = State::HEADER;

		int bytes_since_last_state_change = 0;

		Buffer<buffer_size> buffer;

    };

}

#endif //VALIDATOR_H
