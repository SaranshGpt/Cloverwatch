#ifndef CQUEUE_H
#define CQUEUE_H

#include <array>
#include <atomic>
#include <optional>

#include "c_types.h"

#include <zephyr/kernel.h>

namespace Cloverwatch {

    class CQueue {

    };

    template <typename T, size_t capacity>
    class CQueue_concurrent_SPSC {
    public:

        std::optional<T> pop();
        bool push(T&& val);

        size_t remaining_capacity() const;
        size_t size() const;

        bool empty() const;
        bool full() const;

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

        inline size_t remaining_capacity() const;
        inline size_t size() const;

        inline bool empty() const;
        inline bool full() const;

    private:

        // ReSharper disable once CppUninitializedNonStaticDataMember
        uint8_t _ring_buffer_data[capacity];
        ring_buf ring_buffer = {
            .buffer = _ring_buffer_data,
            .size = capacity
        };

    };

    template <typename T, uint16_t capacity>
    class CQueue_concurrent_MPMC {
    public:

        void init();

        bool push(T&& val);
        void pop(WriteVector<T> &buffer);

        CQueue_concurrent_MPMC() = default;

    private:

        std::array<T, capacity> queue;

        struct k_mutex mutex;

        int head = 0;
        int tail = 0;

    };

}

#include "../src/c_queue.tpp"

#endif //CQUEUE_H