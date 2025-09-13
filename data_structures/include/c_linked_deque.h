//
// Created by saransh on 9/13/25.
//

#ifndef C_LINKED_VECTOR_H
#define C_LINKED_VECTOR_H

#include "c_types.h"
#include "mem_pool.h"

namespace Cloverwatch {

    template <typename T, Heap *heap, size_t standard_block_size>
    class CLinkedDeque {

        struct Node {
            size_t front = 0;
            size_t back = 0;
            size_t size = 0;
            Node* next = nullptr;
            Node* prev = nullptr;
            T vals[];
        };

        Node* front = nullptr;
        Node* back = nullptr;

        static Node* allocate_node();

    public:

        bool push_back(T& val);
        bool pop_back(T& val);

        bool push_front(T& val);
        bool pop_front(T& val);
    };

}

#include "../src/c_linked_deque.tpp"

#endif //C_LINKED_VECTOR_H
