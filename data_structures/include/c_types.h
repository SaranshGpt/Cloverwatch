//
// Created by saransh on 7/28/25.
//

#ifndef C_TYPES_H
#define C_TYPES_H

#include <cstddef>
#include <cstdint>
#include <string.h>
#include <type_traits>
#include <array>

#include "ptr_types.h"

namespace Cloverwatch {

    template <typename T>
    struct Vector;

    template <PtrIntent intent, typename T>
    struct IntentVector {
        T* ptr;
        size_t& len;
        const size_t capacity;

        IntentVector(T* ptr, size_t capacity, size_t &len) : ptr(ptr), len(len), capacity(capacity) {}

        T& operator[](size_t index) { return ptr[index]; }
        const T& operator[](size_t index) const { return ptr[index]; }
        T* data() { return ptr; }
        const T* data() const { return ptr; }
        T* begin() { return ptr; }
        const T* begin() const { return ptr; }
        T* end() { return ptr + len; }
        const T* end() const { return ptr + len; }
        bool empty() const { return len == 0; }
        size_t size() const { return len; }

        void push_back(T val) {
            ptr[len++] = val;
        }

        void pop_back() {
            len--;
        }

        void clear() {
            len = 0;
        }

        const Vector<T> partition(size_t start_ind, size_t end_ind) const {
            size_t new_len = len > end_ind? end_ind - start_ind : len - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_capacity, new_len);
        }

        Vector<T> partition(size_t start_ind, size_t end_ind) {

            size_t new_len = len > end_ind? end_ind - start_ind : len - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_capacity, new_len);
        }

        constexpr const IntentVector<PtrIntent::READONLY, T> to_read() {
            return IntentVector<PtrIntent::READONLY, T>(ptr, capacity, len);
        }

        constexpr IntentVector<PtrIntent::READWRITE, T> to_write() {
            return IntentVector<PtrIntent::READWRITE, T>(ptr, capacity, len);
        }

        constexpr const IntentVector<PtrIntent::BUFFER_READONLY, T> to_buffer_read() {
            return IntentVector<PtrIntent::BUFFER_READONLY, T>(ptr, capacity, len);
        }

        constexpr IntentVector<PtrIntent::BUFFER_READWRITE, T> to_buffer_write() {
            return IntentVector<PtrIntent::BUFFER_READWRITE, T>(ptr, capacity, len);
        }

        constexpr const IntentVector<PtrIntent::COPY_CONTENTS, T> to_copy() {
            return IntentVector<PtrIntent::COPY_CONTENTS, T>(ptr, capacity, len);
        }

    };

    template <typename T>
    using ReadVector = const IntentVector<PtrIntent::READONLY, T>;
    template <typename T>
    using WriteVector = IntentVector<PtrIntent::READWRITE, T>;
    template <typename T>
    using ReadBufferVector = const IntentVector<PtrIntent::BUFFER_READONLY, T>;
    template <typename T>
    using WriteBufferVector = IntentVector<PtrIntent::BUFFER_READWRITE, T>;
    template <typename T>
    using MoveVector = Vector<T>&&;
    template <typename T>
    using CopyVector = const IntentVector<PtrIntent::COPY_CONTENTS, T>;

    template <typename T>
    struct Vector {
        T* ptr;
        size_t len;
        size_t capacity;

        constexpr Vector(T* ptr, size_t capacity, size_t len) : ptr(ptr), len(len), capacity(capacity) {}

        T& operator[](size_t index) { return ptr[index]; }
        const T& operator[](size_t index) const { return ptr[index]; }
        T* data() { return ptr; }
        const T* data() const { return ptr; }
        T* begin() { return ptr; }
        const T* begin() const { return ptr; }
        T* end() { return ptr + len; }
        const T* end() const { return ptr + len; }
        bool empty() const { return len == 0; }
        size_t size() const { return len; }

        void push_back(T val) {
            ptr[len++] = val;
        }

        void pop_back() {
            len--;
        }

        void clear() {
            len = 0;
        }

        T& back() {
            return ptr[len-1];
        }

        const Vector<T> partition(size_t start_ind, size_t end_ind) const {
            size_t new_len = len > end_ind? end_ind - start_ind : len - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_len, new_capacity);
        }

        Vector<T> partition(size_t start_ind, size_t end_ind) {

            size_t new_len = len > end_ind? end_ind - start_ind : len - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_len, new_capacity);
        }

        constexpr ReadVector<T> to_read() {
            return ReadVector<T>(ptr, capacity, len);
        }

        constexpr WriteVector<T> to_write() {
            return WriteVector<T>(ptr, capacity, len);
        }

        constexpr ReadBufferVector<T> to_buffer_read() {
            return ReadBufferVector<T>(ptr, capacity, len);
        }

        constexpr WriteBufferVector<T> to_buffer_write() {
            return WriteBufferVector<T>(ptr, capacity, len);
        }

        constexpr CopyVector<T> to_copy() {
            return CopyVector<T>(ptr, capacity, len);
        }

        constexpr MoveVector<T> to_move() {
            return std::move(*this);
        }

    };

    using Byte = uint8_t;

    template <PtrIntent intent = PtrIntent::READONLY>
    IntentVector<intent, char> intent_string(char* string) {

        size_t size = strlen(string) + 1;
        return IntentVector<intent, char>(string, size);
    }

    template <PtrIntent intent = PtrIntent::READONLY>
    IntentVector<intent, char> intent_string(const char* string) {

        size_t size = strlen(string) + 1;
        return IntentVector<intent, char>(string, size);
    }

    template <typename T, size_t buffer_size>
    struct FixedVector {
        std::array<T, buffer_size> buffer;
        size_t len = 0;

        T& operator[](int index) { return buffer[index]; }
        const T& operator[](int index) const { return buffer[index]; }
        T* data() { return buffer.data(); }
        const T* data() const { return buffer.data(); }
        T* begin() { return buffer.data(); }
        const T* begin() const { return buffer.data(); }
        T* end() { return buffer.data() + len; }
        const T* end() const { return buffer.data() + len; }
        bool empty() const { return len == 0; }
        size_t size() const { return len; }
        T& back() { return buffer[len-1]; }

        void fill(T val, int start_ind = 0, int end_ind = buffer_size) {
            memset(buffer.data() + start_ind, val, end_ind - start_ind);
        }

        void push_back(T val) {
            buffer[len++] = val;
        }

        void pop_back() {
            len--;
        }

        void clear() {
            len = 0;
        }

        constexpr ReadVector<T> to_read() {
            return ReadVector<T>(buffer.data(), buffer_size, len);
        }

        constexpr WriteVector<T> to_write() {
            return WriteVector<T>(buffer.data(), buffer_size, len);
        }

    };

    template <size_t buffer_size>
    using FixedBuffer = FixedVector<Byte, buffer_size>;

}

#endif //C_TYPES_H