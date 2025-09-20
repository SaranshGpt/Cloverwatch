//
// Created by saransh on 9/13/25.
//

#ifndef PATTERN_STATS_H
#define PATTERN_STATS_H

#endif //PATTERN_STATS_H

#include "pattern.h"

#include "../../data_structures/include/c_queue.h"
#include "../../data_structures/include/c_linked_deque.h"
#include "../../data_structures/include/c_types.h"
#include "../../data_structures/include/time.h"
#include "../../data_structures/include/c_mutex.h"

namespace Cloverwatch::Pattern {

    enum class StatResult {
        OK,
        INSUFFICIENT_CAPACITY,
        INVALID_NOTATION
    };

    template <typename G, typename L>
    class StatTracker {

    public:

        StatResult add_packet(ReadVector<Byte> packet);
        StatResult add_pattern(ReadBufferPtr<char> name, ReadVector<Byte> notation);

        void start_process();

        StatTracker() = default;

    private:

        struct PatternInfo {
            char* name = nullptr;
            Pattern pattern;
            size_t num_instances = 0;
            CQueue_concurrent_SPSC<Time, L::history_length> timestamps;
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

        Mutex packet_mtx;
        Mutex pattern_mtx;

        static void packet_process_func(void* instance);
    };

}