//
// Created by saransh on 9/24/25.
//

#ifndef CLOVERWATCH_C_STRING_H
#define CLOVERWATCH_C_STRING_H

#include <stddef.h>

#include "c_types.h"
#include "mem_pool.h"
#include "c_heap_vec.h"

namespace Cloverwatch {

    class String;

    template <PtrIntent intent>
    class IntentString{

        char* ptr;
        size_t& len_;
        const size_t capacity_;

    public:

        IntentString(char* ptr, size_t capacity, size_t &len) : ptr(ptr), len_(len), capacity_(capacity) {}

        char& operator[](size_t index) { return ptr[index]; }
        const char& operator[](size_t index) const { return ptr[index]; }
        char* data() { return ptr; }
        const char* data() const { return ptr; }
        char* begin() { return ptr; }
        const char* begin() const { return ptr; }
        [[nodiscard]] const char* end() const {return ptr + len_ - 1;}
        [[nodiscard]] char* end() {return ptr + len_ - 1;}
        bool empty() const { return len_ <= 1;}
        size_t capacity() const { return capacity_ - 1; }
        size_t len() const { return len_ - 1; }

        void push_back(const char val) {
            ptr[len_-1] = val;
            ptr[len_] = 0;
            len_++;
        }

        void pop_back() {
            len_--;
            ptr[len_] = 0;
        }

        void clear() {
            len_ = 1;
            ptr[0] = 0;
        }

        char& back() {
            return ptr[len_-2];
        }

        void set_len(size_t new_len) { len_ = new_len; }

        constexpr IntentString<PtrIntent::READONLY> to_read() {
            return IntentString<PtrIntent::READONLY>(ptr, capacity_, len_);
        }

        constexpr IntentString<PtrIntent::READWRITE> to_write() {
            return IntentString<PtrIntent::READWRITE>(ptr, capacity_, len_);
        }

        constexpr IntentString<PtrIntent::BUFFER_READONLY> to_buffer_read() {
            return IntentString<PtrIntent::BUFFER_READONLY>(ptr, capacity_, len_);
        }

        constexpr IntentString<PtrIntent::BUFFER_READWRITE> to_buffer_write() {
            return IntentString<PtrIntent::BUFFER_READWRITE>(ptr, capacity_, len_);
        }

        constexpr IntentString<PtrIntent::COPY_CONTENTS> to_copy() {
            return IntentString<PtrIntent::COPY_CONTENTS>(ptr, capacity_, len_);
        }

    };

    class String {
    protected:
        char* ptr;
        size_t len_;
        size_t capacity_;
    public:

        String(): ptr(nullptr), len_(0), capacity_(0) {};

        String(const char* str, size_t capacity, size_t len): ptr(nullptr), len_(len), capacity_(capacity) {}

        char& operator[](size_t index) { return ptr[index]; }
        const char& operator[](size_t index) const { return ptr[index]; }
        char* data() { return ptr; }
        const char* data() const { return ptr; }
        char* begin() { return ptr; }
        const char* begin() const { return ptr; }
        [[nodiscard]] const char* end() const {return ptr + len_ - 1;}
        [[nodiscard]] char* end() {return ptr + len_ - 1;}
        bool empty() const { return len_ <= 1;}
        size_t capacity() const { return capacity_ - 1; }
        size_t len() const { return len_ - 1; }

        void push_back(const char val) {
            ptr[len_-1] = val;
            ptr[len_] = 0;
            len_++;
        }

        void pop_back() {
            len_--;
            ptr[len_] = 0;
        }

        char& back() {
            return ptr[len_-2];
        }

        bool operator==(const String& other) const {

            if (len_ != other.len_) {
                return false;
            }

            for (size_t i=0; i<len_; i++) {
                if (ptr[i] != other.ptr[i]) return false;
            }

            return true;
        }
    };

    using ReadStr = const IntentString<PtrIntent::READONLY>;
    using WriteStr = IntentString<PtrIntent::READWRITE>;
    using BufferReadStr = const IntentString<PtrIntent::BUFFER_READONLY>;
    using BufferWriteStr = IntentString<PtrIntent::BUFFER_READWRITE>;
    using CopyStr = const IntentString<PtrIntent::COPY_CONTENTS>;

}

#endif //CLOVERWATCH_C_STRING_H