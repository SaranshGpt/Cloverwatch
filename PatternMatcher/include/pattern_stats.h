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

namespace Cloverwatch::Pattern {

    template <size_t max_patterns, size_t history_length>
    class StatTracker {

    public:

        enum class Result {
            OK,
            INSUFFICIENT_CAPACITY,
            INVALID_NOTATION
        };

        inline size_t remaining_capacity() const;
        inline size_t size() const;

        Result add_packet(ReadVector<Byte> packet);
        Result add_pattern(ReadPtr<char> name, ReadVector<Byte> notation);

    private:

        struct PatternInfo {

        };

        CByteQueue_SPSC<packet_buffer_size> packet_queue;
        FixedBuffer<packet_buffer_size> packet_buffer;

        StatTracker();
    };

}