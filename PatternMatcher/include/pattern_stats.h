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
        PATTERN_NOT_ENABLED,
        PATTERN_ALREADY_ENABLED,
        PATTERN_NOT_DEFINED,
        PATTERN_ALREADY_DEFINED,
    };

    template <typename G, typename L>
    class StatTracker {
    public:

        StatResult add_packet(ReadVector<Byte> packet);

        StatResult add_pattern(CopyStr name, ReadVector<Byte> notation);

        StatResult get_pattern_info(ReadStr name, WriteRef<size_t> num_instances, WriteVector<Time> timestamps);

        StatResult set_pattern_enabled(ReadStr name, bool enabled);

        void start_process();

        StatTracker()=default;

    private:

        struct PatternHistory {
            size_t num_instances = 0;
            FixedVector<Time, L::history_length> timestamps;
        };

        struct PatternInfo {
            PatternStr name;
            Pattern pattern;
            size_t num_instances = 0;
            CQueue<Time, L::history_length> timestamps;

            std::atomic<bool> enabled{false};
            std::atomic<bool> defined{false};

            Mutex mtx;
        };

        enum class PacketStep {
            MSB,
            LSB,
            PACKET
        };

        struct PacketState {
            FixedBuffer<G::max_packet_size> packet_buffer;
            size_t expected_bytes;
            PacketStep step;
        } packet_state;

        CLinkedDeque<Byte, &pattern_heap, 128> packet_queue;
        FixedVector<PatternInfo, L::max_patterns> patterns;

        Mutex packet_mtx;

        static void process_thread_func(void* instance);
    };

}

#endif //PATTERN_STATS_H