#ifndef CQUEUE_H
#define CQUEUE_H

#include <array>
#include <atomic>
#include <optional>

#include "c_types.h"

#include <zephyr/kernel.h>

#include "circular_count.h"
#include "c_mutex.h"

namespace Cloverwatch {

    template <typename T, size_t capacity>
    class CQueue {
    public:

        std::optional<T> pop() {
            if (head == tail) {
                return std::nullopt;
            }

            auto ret = queue[*tail];
            ++tail;
            return ret;
        }
        bool push(T val, bool overwrite = false) {
            if (full()) {
                if (!overwrite) return false;
                pop();
            }

            queue[*head] = val;
            ++head;
            return true;
        }

        [[nodiscard]] size_t remaining_capacity() const {
            return capacity - size();
        }
        [[nodiscard]] size_t size() const {
            return (*tail - *head + capacity)%capacity;
        }

        [[nodiscard]] bool empty() const {
            return (head == tail);
        }
        [[nodiscard]] bool full() const {
            return size() == capacity;
        }

        void clear() {
            *head = 0;
            *tail = 0;
        }

        FixedVector<T, capacity> get_vec() {
            FixedVector<T, capacity> ret;

            CCount ind = head;
            while (ind != tail) {
                ret.push_back(queue[*ind]);
                ++ind;
            }

            return ret;
        }

    private:

        std::array<T, capacity> queue;
        CCount<size_t, capacity> head;
        CCount<size_t, capacity> tail;
    };

    template <typename T, size_t capacity>
    class CQueue_concurrent_SPSC {
    public:

        std::optional<T> pop();
        bool push(T&& val, bool overwrite = false);

        size_t remaining_capacity() const;
        size_t size() const;

        bool empty() const;
        bool full() const;

        void clear() {
            head = 0;
            tail = 0;
        }

    private:

        std::array<T, capacity> queue;
        std::atomic<uint16_t> head = 0;
        std::atomic<uint16_t> tail = 0;

        static_assert(capacity > 0, "Invalid value of size");

    };

    template <size_t capacity>
    class CByteQueue_SPSC {
    public:

        constexpr CByteQueue_SPSC() = default;

        bool push(Byte byte);
        bool push(ReadVector<Byte> bytes);

        std::optional<Byte> pop();
        void pop(WriteVector<Byte> buffer);

        [[nodiscard]] [[gnu::always_inline]] inline size_t remaining_capacity() const;
        [[nodiscard]] [[gnu::always_inline]] inline size_t size() const;

        [[nodiscard]] [[gnu::always_inline]] inline bool empty() const;
        [[nodiscard]] [[gnu::always_inline]] inline bool full() const;

    private:

        // ReSharper disable once CppUninitializedNonStaticDataMember
        uint8_t _ring_buffer_data[capacity];
        ring_buf ring_buffer = {
            .buffer = _ring_buffer_data,
            .size = capacity
        };

    };

    template <typename T, size_t capacity>
    class CQueue_concurrent_MPMC {
    public:

        void init();

        bool push(T&& val);

        std::optional<T> pop();
        std::optional<T> displace(T&& val);

        CQueue_concurrent_MPMC() = default;

    private:

        std::array<T, capacity> queue;

        Mutex mtx;

        CCount<size_t, capacity> front_ind;
        CCount<size_t, capacity> back_ind;
    };

}

#include "../src/c_queue.tpp"

#endif //CQUEUE_H