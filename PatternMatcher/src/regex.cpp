//
// Created by saransh on 9/5/25.
//

#include "regex.h"

#include <cstdint>


namespace Cloverwatch::Regex {

    static uint16_t get_uint16(ReadVector<Byte> packet, int offset) {

        uint16_t val = 0;

        val |= packet[offset];
        val <<= 8;
        val |= packet[offset+1];

        return val;
    }

    static enum class RegexResult {
        NO_MATCH,
        MATCH_RECURSE

    };

    static bool match_regex(ReadVector<Byte> pattern, ReadVector<Byte> packet, int data_start, int data_end, int regex_start, int regex_end) {



    }

    bool match(Pattern pattern, Packet packet) {

        int data_start = 0;
        int pattern_start = 0;

        for (int i=0; i< RegexConfig::max_depth; i++) {

            auto range_start = get_uint16(pattern, pattern_start);

            if (range_start == 0xFFFF) return true;
            if (range_start == 0) return false;

            auto range_end = get_uint16(pattern, pattern_start + 2);

            auto regex_start = get_uint16(pattern, pattern_start + 4);
            auto regex_end = get_uint16(pattern, pattern_start + 6);

            auto positive_start = get_uint16(pattern, pattern_start + 8);
            auto positive_end = get_uint16(pattern, pattern_start + 10);

            auto negative_start = get_uint16(pattern, pattern_start + 12);
            auto negative_end = get_uint16(pattern, pattern_start + 14);

            data_start += range_start;

            if (match_regex(pattern, packet, data_start + range_start, data_start + range_end, pattern_start + regex_start, pattern_start + regex_end))
                pattern_start += positive_start;
            else pattern_start += negative_start;
        }

    }

}
