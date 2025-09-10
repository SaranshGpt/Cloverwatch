//
// Created by saransh on 9/10/25.
//

#ifndef MEM_POOL_H
#define MEM_POOL_H

#include <zephyr/kernel.h>

namespace Cloverwatch {

    class Heap {

        k_heap* heap;
        const char* name;

    public:

        constexpr Heap(k_heap* heap, const char* name): heap(heap), name(name) {}

        template <typename T>
        T* allocate(const size_t num = 1) {
            return static_cast<T*>(k_heap_alloc(heap, sizeof(T) * num, K_NO_WAIT));
            //TODO: Add error handling
        }

        void free(void* ptr) {
            k_heap_free(heap, ptr);
        }

        const char* get_name() const { return name;}
    };

#define DECLARE_HEAP(name) \
    extern Heap name

#define DEFINE_HEAP(name, size) \
    K_HEAP_DEFINE(name##_source_heap, size); \
    Heap name(&name##_source_heap, #name); \

}

#endif //MEM_POOL_H
