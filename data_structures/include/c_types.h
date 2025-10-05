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
    class Vector {

    protected:

        T* ptr;
        size_t len_;
        size_t capacity_;

    public:

        constexpr Vector(): ptr(nullptr), len_(0), capacity_(0) {}
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

        void set_len(size_t len) {
            len_ = len;
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

    };

    using Byte = uint8_t;

    template <typename T, size_t buffer_size>
    class FixedVector: public Vector<T> {
        T buffer[buffer_size];

    public:

        FixedVector(): Vector<T>(buffer, buffer_size, 0) {}

        Vector<T>& as_vec() {
            return *this;
        }
    };

    template <size_t buffer_size>
    using FixedBuffer = FixedVector<Byte, buffer_size>;

    template <typename T>
    using ReadVector = ReadRef<Vector<T>>;

    template <typename T>
    using WriteVector = WriteRef<Vector<T>>;

    template <typename T>
    using ReadBufferVector = ReadBufferRef<Vector<T>>;

    template <typename T>
    using WriteBufferVector = WriteBufferRef<Vector<T>>;

    template <typename T>
    using CopyVector = CopyRef<Vector<T>>;

}

#endif //C_TYPES_H