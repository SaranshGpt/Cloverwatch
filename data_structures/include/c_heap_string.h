//
// Created by saransh on 9/24/25.
//

#ifndef CLOVERWATCH_C_HEAP_STRING_H
#define CLOVERWATCH_C_HEAP_STRING_H

#include <cstddef>

#include "c_string.h"
#include "mem_pool.h"

namespace Cloverwatch {

    template <Heap *heap>
    class HeapString: public String {
    public:

        HeapString(): String() {}

        explicit HeapString(CopyRef<char*> str, const size_t excess_capacity = 0) {

            len_ = strlen(str.ref) + 1;
            capacity_ = len_ + excess_capacity;

            ptr = heap->allocate<char>(capacity_);

            if (ptr == nullptr) {
                len_ = 0;
                capacity_ = 0;
                return;
            }

            bytecpy(ptr, str.ref, len_);
        }

        static HeapString<heap> copy_string(CopyRef<String> str) {
            auto heap_str = HeapString();
            heap_str.ptr = heap->allocate<char>(str.ref.capacity());

            if (heap_str.ptr == nullptr) {
                return HeapString();
            }

            bytecpy(heap_str.begin(), str.ref.begin(), str.ref.len());

            heap_str.len_ = str.ref.len();
            heap_str.capacity_ = str.ref.capacity();

            return heap_str;
        }

        bool realloc(size_t new_capacity) {
            char* new_ptr = heap->allocate<char>(new_capacity+1);

            if (new_ptr == nullptr) {
                return false;
            }

            if (this->ptr != nullptr) {
                memcpy(new_ptr, this->ptr, this->len_);
                heap->free(this->ptr);
            }

            this->ptr = new_ptr;
            this->capacity_ = new_capacity+1;

            return true;
        }

        void free() {
            heap->free(this->ptr);
            this->ptr = nullptr;
            this->capacity_ = 0;
            this->len_ = 0;
        }

        constexpr String& as_str() {
            return *this;
        }
    };

}

#endif //CLOVERWATCH_C_HEAP_STRING_H