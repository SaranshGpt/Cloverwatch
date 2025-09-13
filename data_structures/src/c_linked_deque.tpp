//
// Created by saransh on 9/13/25.
//

#ifndef C_LINKED_DEQUE_TPP
#define C_LINKED_DEQUE_TPP

#include "../include/c_linked_deque.h"

namespace Cloverwatch {

#define CLinkedNode typename CLinkedDeque<T, heap, standard_block_size>::Node

    template <typename T, Heap* heap, size_t standard_block_size>
    CLinkedNode* CLinkedDeque<T, heap, standard_block_size>::allocate_node() {

        constexpr size_t t_align = alignof(T);

        constexpr size_t aligned_header_size = (sizeof(Node) + t_align - 1) & ~(t_align - 1);

        constexpr size_t min_size = aligned_header_size + sizeof(T);

        size_t curr_size = standard_block_size;

        void* mem_block = nullptr;

        while ( mem_block == nullptr && curr_size >= min_size ) {
            mem_block = heap->allocate<void>(curr_size);
            curr_size >>= 1;
        }

        if (mem_block == nullptr) return nullptr;

        Node* node = new (mem_block) Node();
        node->size = (curr_size - aligned_header_size)/sizeof(T);

        return node;
    }

    template <typename T, Heap* heap, size_t standard_block_size>
    bool CLinkedDeque<T, heap, standard_block_size>::push_back(T& val) {

        if (!front) {
            front = allocate_node();
        }

    }

#undef CLinkedNode

}

#endif //C_LINKED_DEQUE_TPP
