//
// Created by saransh on 8/4/25.
//

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "reed_soloman.h"
#include "../../data_structures/include/c_types.h"

namespace Cloverwatch {

	using BlockEccFunc = bool (*)(WriteVector<Byte> message_rx);

	//TODO: Implement Header CRC for Block Validator.  Right now there is nothing handling erros in header values. Skipping this now as the header is only length as of now

    template <typename G, typename L, BlockEccFunc encode_func, BlockEccFunc decode_func>
    class SimplePacketiser_Block {
	public:

		void add_byte(Byte byte, WriteVector<Byte> message_tx);

    	void reset();

    	void construct_packet(ReadVector<Byte> payload, WriteVector<Byte> packet) const;

	private:

		enum class State {
			HEADER,
			LENGTH,
			PAYLOAD,
			FOOTER
		} curr_state = State::HEADER;

		size_t bytes_since_last_state_change = 0;
    	bool escape_expected = false;

    	uint64_t expected_length = 0;

		FixedBuffer<G::max_packet_size> buffer;

    	void append_length_byte(Byte byte);

    };

}

#include "../src/simple_packetiser.tpp"

#endif //VALIDATOR_H
