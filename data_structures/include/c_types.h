//
// Created by saransh on 7/28/25.
//

#ifndef C_TYPES_H
#define C_TYPES_H

#include "ptr_types.h"
#include <cstddef>
#include <string.h>

#include "ptr_types.h"

namespace Cloverwatch {

    template <PtrIntent intent, typename T>
    struct IntentVector {

        IntentPtr<intent, T> ptr;
        size_t &len;
        const size_t capacity;

        constexpr IntentVector(IntentPtr<intent, T> ptr, size_t capacity, size_t &len) : ptr(ptr), len(len), capacity(capacity) {}
        constexpr IntentVector(T* ptr, size_t capacity, size_t &len) : ptr(IntentPtr<intent, T>(ptr)), len(len), capacity(capacity) {}

        template <PtrIntent other>
        constexpr std::enable_if_t<
            other != PtrIntent::READONLY &&
            other != PtrIntent::COPY_CONTENTS &&
            other != PtrIntent::BUFFER_READONLY,
            IntentVector<other, T>
        > to_intent() {
            return IntentVector<other, T>(ptr.ptr, capacity, len);
        }

        template<PtrIntent other>
        constexpr std::enable_if_t<
            other == PtrIntent::READONLY ||
            other == PtrIntent::COPY_CONTENTS ||
            other == PtrIntent::BUFFER_READONLY,
            const IntentVector<other, const T>
        > to_intent() {
            return IntentVector<other, const T>((const T*)ptr.ptr, capacity, len);
        }

        constexpr const IntentVector<PtrIntent::READONLY, const T> to_read_vector() { return to_intent<PtrIntent::READONLY>(); }
        constexpr IntentVector<PtrIntent::READWRITE, T> to_write_vector() { return to_intent<PtrIntent::READWRITE>(); }
        constexpr const IntentVector<PtrIntent::BUFFER_READONLY, const T> to_buffer_read_vector() { return to_intent<PtrIntent::BUFFER_READONLY>(); }
        constexpr IntentVector<PtrIntent::BUFFER_READWRITE, T> to_buffer_write_vector() { return to_intent<PtrIntent::BUFFER_READWRITE>(); }
        constexpr IntentVector<PtrIntent::MOVE_OWNERSHIP, T> to_move_vector() { return to_intent<PtrIntent::MOVE_OWNERSHIP>(); }
        constexpr const IntentVector<PtrIntent::COPY_CONTENTS, const T> to_copy_vector() { return to_intent<PtrIntent::COPY_CONTENTS>(); }

        void push_back(T val) {
            if (len >= capacity) {
                return;
            }
            ptr.ptr[len++] = val;
        }

        void pop_back() {
            if (len == 0) {
                return;
            }
            len--;
        }

        void clear() {
            len = 0;
        }

        constexpr T* data() { return ptr.ptr; }
        constexpr const T* data() const { return ptr.ptr; }
        constexpr T& operator[](size_t index) { return ptr.ptr[index]; }
        constexpr const T& operator[](size_t index) const { return ptr.ptr[index]; }
        constexpr T* begin() { return ptr.ptr; }
        constexpr const T* begin() const { return ptr.ptr; }
        constexpr T* end() { return ptr.ptr + len; }
        constexpr const T* end() const { return ptr.ptr + len; }
        constexpr bool empty() const { return len == 0; }
        constexpr size_t size() const { return len; }
    };

    template <typename T>
    using ReadVector = const IntentVector<PtrIntent::READONLY, const T>;
    template <typename T>
    using WriteVector = IntentVector<PtrIntent::READWRITE, T>;
    template <typename T>
    using ReadBufferVector = const IntentVector<PtrIntent::BUFFER_READONLY, const T>;
    template <typename T>
    using WriteBufferVector = IntentVector<PtrIntent::BUFFER_READWRITE, T>;
    template <typename T>
    using MoveVector = IntentVector<PtrIntent::MOVE_OWNERSHIP, T>;
    template <typename T>
    using CopyVector = const IntentVector<PtrIntent::COPY_CONTENTS, const T>;

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

    template <size_t buffer_size>
    struct Buffer {
        std::array<Byte, buffer_size> buffer;
        size_t len = 0;

        Byte& operator[](int index) { return buffer[index]; }
        const Byte& operator[](int index) const { return buffer[index]; }
        Byte* data() { return buffer.data(); }
        const Byte* data() const { return buffer.data(); }
        Byte* begin() { return buffer.data(); }
        const Byte* begin() const { return buffer.data(); }
        Byte* end() { return buffer.data() + len; }
        const Byte* end() const { return buffer.data() + len; }
        bool empty() const { return len == 0; }
        size_t size() const { return len; }

        void fill(Byte val, int start_ind = 0, int end_ind = buffer_size) {
            memset(buffer.data() + start_ind, val, end_ind - start_ind);
        }

        void push_back(Byte val) {
            buffer[len++] = val;
        }

        void pop_back() {
            len--;
        }

        void clear() {
            len = 0;
        }

        constexpr ReadVector<Byte> to_ReadVector() {
            return ReadVector<Byte>(buffer.data(), buffer_size, len);
        }

        constexpr WriteVector<Byte> to_WriteVector() {
            return WriteVector<Byte>(buffer.data(), buffer_size, len);
        }

    };

}

#endif //C_TYPES_H