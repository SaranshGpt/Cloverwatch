//
// Created by saransh on 9/20/25.
//

#ifndef CLOVERWATCH_C_HEAP_VEC_H
#define CLOVERWATCH_C_HEAP_VEC_H

#include "c_types.h"
#include "mem_pool.h"

namespace Cloverwatch {
    template <typename T, Heap *heap>
   struct HeapVector: Vector<T> {
        HeapVector(size_t capacity, size_t len) : Vector<T>(nullptr, capacity, len) {
            this->ptr = heap->allocate<T>(capacity);
            if (this->ptr == nullptr) {
                this->len = 0;
                this->capacity = 0;
            }
        }
        HeapVector(size_t capacity) : HeapVector(capacity, 0) {}
        HeapVector() : Vector<T>(nullptr, 0, 0) {}

        bool realloc(size_t new_capacity) {
            T* new_ptr = heap->allocate<T>(new_capacity);

            if (new_ptr == nullptr) {
                return false;
            }

            if (this->ptr != nullptr) {
                memcpy(new_ptr, this->ptr, this->len * sizeof(T));
                heap->free(this->ptr);
            }

            this->ptr = new_ptr;
            this->capacity = new_capacity;

            return true;
        }

        void free() {
            heap->free(Vector<T>::ptr);
            this->ptr = nullptr;
            this->capacity = 0;
            this->len = 0;
        }
    };
}


#endif //CLOVERWATCH_C_HEAP_VEC_H