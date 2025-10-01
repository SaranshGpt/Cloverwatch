//
// Created by saransh on 9/5/25.
//

#ifndef REGEX_H
#define REGEX_H

#include <optional>
#include <zephyr/kernel.h>

#include "../../data_structures/include/c_types.h"
#include "../../data_structures/include/c_heap_vec.h"
#include "../../data_structures/include/mem_pool.h"
#include "../../system_config.h"
#include "../../data_structures/include/c_heap_string.h"

namespace Cloverwatch::Pattern {

    DECLARE_HEAP(pattern_heap);

    template <typename T>
    using PatternVec = HeapVector<T, &pattern_heap>;

    using PatternStr = HeapString<&pattern_heap>;

    enum class ConditionType {
        FIXED,
        RANGE,
        SELECTION,
        NFIXED,
        NRANGE,
        NSELECTION,
    };

    enum class OperationType {
        PUSH,
        OR,
        AND,
        NOR,
        NAND,
        XOR,
        XNOR
    };

    struct Condition {
        size_t start_range;
        size_t end_range;
        ConditionType type;

        PatternVec<Byte> vals;
    };

    struct Pattern {
        PatternVec<OperationType> operations;
        PatternVec<Condition> conditions;

        void free_memory() {
            for (auto &condition: conditions)
                condition.vals.free();

            conditions.free();
            operations.free();
        }
    };

    bool match_pattern(ReadVector<Byte> data, Pattern& pattern);

    std::optional<Pattern> compile_pattern(ReadVector<Byte> notation);

}

#endif //REGEX_H
