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
    class Vector;

    template <PtrIntent intent, typename T>
    class IntentVector {
    protected:

        T* ptr;
        size_t* len_;
        const size_t capacity_;

    public:

        IntentVector(T* ptr, size_t capacity, size_t *len) : ptr(ptr), len_(len), capacity_(capacity) {}
        IntentVector(): ptr(nullptr), len_(nullptr), capacity_(0) {}

        T& operator[](size_t index) { return ptr[index]; }
        const T& operator[](size_t index) const { return ptr[index]; }
        T* data() { return ptr; }
        const T* data() const { return ptr; }
        T* begin() { return ptr; }
        const T* begin() const { return ptr; }
        T* end() { return ptr + *len_; }
        const T* end() const { return ptr + *len_; }
        [[nodiscard]] bool empty() const { return *len_ == 0; }
        [[nodiscard]] size_t len() const { return *len_; }
        [[nodiscard]] size_t capacity() const {return capacity_;}

        void push_back(T val) {
            ptr[*len_++] = val;
        }

        void pop_back() const {
            (*len_)--;
        }

        void clear() const {
            *len_ = 0;
        }

        [[nodiscard]] inline bool is_valid() const {
            return ptr != nullptr && len_ != nullptr;
        }

        void invalidate() {
            ptr = nullptr;
            len_ = nullptr;
        }

        Vector<T> partition(size_t start_ind, size_t end_ind) const {
            size_t new_len = *len_ > end_ind ? end_ind - start_ind : *len_ - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_capacity, new_len);
        }

        Vector<T> partition(size_t start_ind, size_t end_ind) {

            size_t new_len = *len_ > end_ind? end_ind - start_ind : *len_ - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_capacity, new_len);
        }

        void set_len(size_t new_len) { *len_ = new_len; }

        constexpr IntentVector<PtrIntent::READONLY, T> to_read() {
            return IntentVector<PtrIntent::READONLY, T>(ptr, capacity_, len_);
        }

        constexpr IntentVector<PtrIntent::READWRITE, T> to_write() {
            return IntentVector<PtrIntent::READWRITE, T>(ptr, capacity_, len_);
        }

        constexpr IntentVector<PtrIntent::BUFFER_READONLY, T> to_buffer_read() {
            return IntentVector<PtrIntent::BUFFER_READONLY, T>(ptr, capacity_, len_);
        }

        constexpr IntentVector<PtrIntent::BUFFER_READWRITE, T> to_buffer_write() {
            return IntentVector<PtrIntent::BUFFER_READWRITE, T>(ptr, capacity_, len_);
        }

        constexpr IntentVector<PtrIntent::COPY_CONTENTS, T> to_copy() {
            return IntentVector<PtrIntent::COPY_CONTENTS, T>(ptr, capacity_, len_);
        }

    };

    template <typename T>
    using ReadVector = const IntentVector<PtrIntent::READONLY, T>;
    template <typename T>
    using WriteVector = IntentVector<PtrIntent::READWRITE, T>;
    template <typename T>
    using BufferReadVector = const IntentVector<PtrIntent::BUFFER_READONLY, T>;
    template <typename T>
    using BufferWriteVector = IntentVector<PtrIntent::BUFFER_READWRITE, T>;
    template <typename T>
    using MoveVector = Vector<T>&&;
    template <typename T>
    using CopyVector = const IntentVector<PtrIntent::COPY_CONTENTS, T>;

    template <typename T>
    class Vector {

    protected:

        T* ptr;
        size_t len_;
        size_t capacity_;

    public:

        constexpr Vector(T* ptr, size_t capacity, size_t len) : ptr(ptr), len_(len), capacity_(capacity) {}

        T& operator[](size_t index) { return ptr[index]; }
        const T& operator[](size_t index) const { return ptr[index]; }
        T* data() { return ptr; }
        const T* data() const { return ptr; }
        T* begin() { return ptr; }
        const T* begin() const { return ptr; }
        T* end() { return ptr + len_; }
        const T* end() const { return ptr + len_; }
        [[nodiscard]] bool empty() const { return len_ == 0; }

        [[nodiscard]] size_t capacity() const { return capacity_; }
        [[nodiscard]] size_t len() const { return len_; }

        void push_back(T val) {
            ptr[len_++] = val;
        }

        void pop_back() {
            len_--;
        }

        void clear() {
            len_ = 0;
        }

        T& back() {
            return ptr[len_-1];
        }

        Vector<T> partition(size_t start_ind, size_t end_ind) const {
            size_t new_len = len_ > end_ind? end_ind - start_ind : len_ - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_len, new_capacity);
        }

        Vector<T> partition(size_t start_ind, size_t end_ind) {

            size_t new_len = len_ > end_ind? end_ind - start_ind : len_ - start_ind;
            size_t new_capacity = end_ind - start_ind;

            return Vector<T>(ptr + start_ind, new_len, new_capacity);
        }

        constexpr ReadVector<T> to_read() {
            return ReadVector<T>(ptr, capacity_, &len_);
        }

        constexpr WriteVector<T> to_write() {
            return WriteVector<T>(ptr, capacity_, &len_);
        }

        constexpr BufferReadVector<T> to_buffer_read() {
            return BufferReadVector<T>(ptr, capacity_, &len_);
        }

        constexpr BufferWriteVector<T> to_buffer_write() {
            return BufferWriteVector<T>(ptr, capacity_, &len_);
        }

        constexpr CopyVector<T> to_copy() {
            return CopyVector<T>(ptr, capacity_, &len_);
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
    class FixedVector {
        std::array<T, buffer_size> buffer;
        size_t len = 0;

    public:

        T& operator[](int index) { return buffer[index]; }
        const T& operator[](int index) const { return buffer[index]; }
        T* data() { return buffer.data(); }
        const T* data() const { return buffer.data(); }
        T* begin() { return buffer.data(); }
        const T* begin() const { return buffer.data(); }
        T* end() { return buffer.data() + len; }
        const T* end() const { return buffer.data() + len; }
        [[nodiscard]] bool empty() const { return len == 0; }
        [[nodiscard]] size_t size() const { return len; }
        T& back() { return buffer[len-1]; }

        void fill(T val, size_t start_ind = 0, size_t end_ind = buffer_size) {
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
            return ReadVector<T>(buffer.data(), buffer_size, &len);
        }

        constexpr WriteVector<T> to_write() {
            return WriteVector<T>(buffer.data(), buffer_size, &len);
        }

    };

    template <size_t buffer_size>
    using FixedBuffer = FixedVector<Byte, buffer_size>;

}

#endif //C_TYPES_H