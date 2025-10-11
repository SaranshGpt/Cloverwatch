//
// Created by saransh on 9/24/25.
//

#ifndef CLOVERWATCH_C_STRING_H
#define CLOVERWATCH_C_STRING_H

#include <stddef.h>

#include "c_types.h"
#include "mem_pool.h"
#include "c_heap_vec.h"
#include "c_string.h"

namespace Cloverwatch {

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

        void set_len(size_t len) {
            len_ = len;
            ptr[len_] = 0;
        }

        void clear() {
            len_ = 0;
            ptr[0] = 0;
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

        bool operator!=(const String& other) const {
            return !(*this == other);
        }
    };

    using ReadStr = ReadRef<String>;
    using WriteStr = WriteRef<String>;
    using ReadBufferStr = ReadBufferRef<String>;
    using WriteBufferStr = WriteBufferRef<String>;
    using CopyStr = CopyRef<String>;

    template <size_t buffer_size>
    class FixedStr: public String {

        char buffer[buffer_size];

    public:

        FixedStr(): String(buffer, buffer_size, 0) {}

        template <size_t T>
        constexpr FixedStr(const char (&str)[T]) : String(str, buffer_size, T-1) {}

        String& as_str() {
            return *this;
        }
        
    };

}

#endif //CLOVERWATCH_C_STRING_H