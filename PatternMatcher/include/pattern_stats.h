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

namespace Cloverwatch::Pattern {

    enum class StatResult {
        OK,
        INSUFFICIENT_CAPACITY,
        INVALID_NOTATION
    };

    template <size_t max_patterns, size_t history_length>
    class StatTracker {

    public:

        inline size_t remaining_capacity() const;
        inline size_t size() const;

        StatResult add_packet(ReadVector<Byte> packet);
        StatResult add_pattern(ReadPtr<char> name, ReadVector<Byte> notation);

        StatTracker() = default;

    private:

        struct PatternInfo {
            char* name = nullptr;
            Pattern pattern;
            size_t num_instances = 0;
            CQueue_concurrent_SPSC<Time, history_length> timestamps;
        };

        CLinkedDeque<Byte, &pattern_heap, 128> PacketQueue;
        FixedVector<PatternInfo, max_patterns> patterns;

        static void ProcessFunc(void* instance);
    };

}