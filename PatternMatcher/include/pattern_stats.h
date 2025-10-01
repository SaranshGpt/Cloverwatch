//
// Created by saransh on 9/13/25.
//

#ifndef PATTERN_STATS_H
#define PATTERN_STATS_H

#include <cstddef>

#include "pattern.h"

#include "../../data_structures/include/c_queue.h"
#include "../../data_structures/include/c_linked_deque.h"
#include "../../data_structures/include/c_types.h"
#include "../../data_structures/include/c_string.h"
#include "../../data_structures/include/time.h"
#include "../../data_structures/include/c_mutex.h"

namespace Cloverwatch::Pattern {

    enum class StatResult {
        OK,
        INSUFFICIENT_CAPACITY,
        INVALID_NOTATION,
        REQUEST_ALREADY_EXISTS,
        REQUEST_NOT_FULFILLED,
        REQUEST_DOES_NOT_EXIST,
        PATTERN_DOES_NOT_EXIST,
        PATTERN_ALREADY_EXISTS,
        PATTERN_NOT_ENABLED,
        PATTERN_ALREADY_ENABLED,
        PATTERN_NOT_DEFINED,
        PATTERN_ALREADY_DEFINED,
        PATTERN_NOT_FOUND,
        PATTERN_FOUND,
        PATTERN_NOT_FOUND_IN_RANGE,
    };

    template <typename G, typename L>
    class StatTracker {

    public:

        struct StatRequest {
            BufferReadStr name;
            size_t num_instances = 0;
            BufferWriteVector<Time> timestamps;
            StatResult result = StatResult::REQUEST_NOT_FULFILLED;
        };

        StatResult add_packet(ReadVector<Byte> packet);

        StatResult add_pattern(CopyStr name, ReadVector<Byte> notation);

        StatResult add_stat_request(BufferWriteVector<StatRequest> stat_requests);
        bool clear_if_stat_request_fulfilled();

        StatResult set_pattern_enabled(ReadStr name, bool enabled);

        void start_process();

        StatTracker() = default;

    private:

        struct PatternInfo {
            PatternStr name;
            Pattern pattern;
            size_t num_instances = 0;
            CQueue_concurrent_SPSC<Time, L::history_length> timestamps;

            bool enabled = false;
            bool defined = false;
        };

        enum class PacketStep {
            MSB,
            LSB,
            PACKET
        };

        struct PacketState {
            FixedBuffer<G::max_packet_size> packet_buffer;
            uint16_t expected_bytes;
            PacketStep step;
        } packet_state;

        CLinkedDeque<Byte, &pattern_heap, 128> packet_queue;
        FixedVector<PatternInfo, L::max_patterns> patterns;

        BufferWriteVector<StatRequest> current_request;

        Mutex packet_mtx;
        Mutex pattern_mtx;
        Mutex request_mtx;

        static void packet_process_func(void* instance);
    };

}

#endif //PATTERN_STATS_H