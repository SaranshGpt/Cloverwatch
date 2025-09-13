//
// Created by saransh on 6/15/25.
//

// ReSharper disable once CppMissingIncludeGuard
#include "logger.h"
#include "../include/c_queue.h"

namespace Cloverwatch {

    static inline void circular_increment(size_t& val, const size_t &max) {
        ++val;
        val *= (val != max);
    }


    template <typename T, size_t capacity>
    std::optional<T> CQueue_concurrent_SPSC<T, capacity>::pop() {

        if (empty()) return std::nullopt;

        size_t tail_copy = tail.load();
        size_t next_tail = tail.load();
        circular_increment(next_tail, capacity);

        tail = next_tail;

        return std::move(queue[tail_copy]);
    }

    template <typename T, size_t capacity>
    bool CQueue_concurrent_SPSC<T, capacity>::push(T&& val) {
        if (full()) return false;

        size_t head_copy = head.load();
        size_t next_head = head.load();
        circular_increment(next_head, capacity);

        head = next_head;

        queue[head_copy] = std::move(val);

        return true;
    }

    template <typename T, size_t capacity>
    inline bool CQueue_concurrent_SPSC<T, capacity>::empty() const {
        return (tail.load() == head.load());
    }

    template <typename T, size_t capacity>
    inline bool CQueue_concurrent_SPSC<T, capacity>::full() const {
        size_t head_copy = head.load();
        size_t tail_copy = tail.load();

        circular_increment(head_copy, capacity);

        return (head_copy == tail_copy);
    }

    template<typename T, size_t capacity>
    inline size_t CQueue_concurrent_SPSC<T, capacity>::remaining_capacity() const {

        return capacity - size();

    }

    template<typename T, size_t capacity>
    inline size_t CQueue_concurrent_SPSC<T, capacity>::size() const {
        size_t head_copy = head.load();
        size_t tail_copy = tail.load();

        if (head_copy < tail_copy) head_copy += capacity;

        return (head_copy - tail_copy);
    }


    template<size_t capacity>
    bool CByteQueue_SPSC<capacity>::push(Byte byte) {
        return ring_buf_put(&ring_buffer, &byte, 1);
    }

    template<size_t capacity>
    // ReSharper disable once CppPassValueParameterByConstReference
    bool CByteQueue_SPSC<capacity>::push(ReadVector<Byte> bytes) {

        if (remaining_capacity() < bytes.len) return false;

        ring_buf_put(&ring_buffer, bytes.data(), bytes.len);

        return true;
    }

    template <size_t capacity>
    std::optional<Byte> CByteQueue_SPSC<capacity>::pop() {

        Byte byte;

        bool popped = ring_buf_get(&ring_buffer, &byte, 1);

        if (!popped) return std::nullopt;

        return byte;
    }

    template<size_t capacity>
    inline void CByteQueue_SPSC<capacity>::pop(WriteVector<Byte> buffer) {

        buffer.len = ring_buf_get(&ring_buffer, buffer.data(), buffer.capacity);

    }

    template <size_t capacity>
    inline size_t CByteQueue_SPSC<capacity>::remaining_capacity() const {
        return ring_buf_space_get(&ring_buffer);
    }

    template<size_t capacity>
    size_t CByteQueue_SPSC<capacity>::size() const {
        return ring_buf_size_get(&ring_buffer);
    }

    template <size_t capacity>
    inline bool CByteQueue_SPSC<capacity>::empty() const {
        return (size() == 0);
    }

    template <size_t capacity>
    inline bool CByteQueue_SPSC<capacity>::full() const {
        return (size() == capacity);
    }



}