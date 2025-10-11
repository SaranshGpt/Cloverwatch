//
// Created by saransh on 10/8/25.
//

#ifndef CLOVERWATCH_CIRCULAR_COUNT_H
#define CLOVERWATCH_CIRCULAR_COUNT_H

#include <type_traits>

namespace Cloverwatch {

    template <typename T, T max>
    class CCount {
        static_assert(std::is_arithmetic_v<T>, "T must be a numeric type");

        static constexpr T negative_sentinel = T(-1);

        T val = 0;

    public:

        CCount() = default;
        explicit CCount(T val) : val(val) {}

        [[gnu::always_inline]] inline void operator++() {
            ++val;
            val *= (val < max);
        }

        [[gnu::always_inline]] inline void operator--() {
            --val;
            val += (val == negative_sentinel) * max;
        }

        [[gnu::always_inline]] inline T& operator*() {
            return val;
        }

        [[gnu::always_inline]] inline const T& operator*() const {
            return val;
        }

        [[gnu::always_inline]] inline bool operator==(const CCount<T, max>& other) {
            return val == other.val;
        }

        [[gnu::always_inline]] inline bool operator==(const T& other) {
            return val == other.val;
        }

        [[gnu::always_inline]] inline bool operator!=(const CCount<T, max>& other) {
            return val != other.val;
        }

    };

}

#endif //CLOVERWATCH_CIRCULAR_COUNT_H