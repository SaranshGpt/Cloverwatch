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
        size_t len;
        size_t capacity;

        constexpr IntentVector(IntentPtr<intent, T> ptr, size_t capacity, size_t len = 0) : ptr(ptr), len(len), capacity(capacity) {}
        constexpr IntentVector(T* ptr, size_t capacity, size_t len = 0) : ptr(IntentPtr<intent, T>(ptr)), len(len), capacity(capacity) {}

        void push_back(T val) {
            if (len == capacity) {
                return;
            }
            ptr[len++] = val;
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
        constexpr T& operator[](size_t index) { return ptr[index]; }
        constexpr const T& operator[](size_t index) const { return ptr[index]; }
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

    using Byte = std::byte;

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

    template <uint16_t buffer_size>
    struct Buffer {
        std::array<Byte, buffer_size> buffer;
        uint16_t size = 0;

        void fill(Byte val, int start_ind = 0, int end_ind = buffer_size) {
            memset(buffer.data() + start_ind, val, end_ind - start_ind);
        }

        void push_back(Byte val) {
            buffer[size++] = val;
        }

        void pop_back() {
            size--;
        }

        constexpr ReadVector<uint8_t> to_ReadVector() {
            return ReadVector<uint8_t>(buffer.data(), buffer_size, size);
        }

        constexpr WriteVector<uint8_t> to_WriteVector() {
            return WriteVector<uint8_t>(buffer.data(), buffer_size, size);
        }

    };

}

#endif //C_TYPES_H