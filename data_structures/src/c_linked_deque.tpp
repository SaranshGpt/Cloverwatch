//
// Created by saransh on 9/13/25.
//

#ifndef C_LINKED_DEQUE_TPP
#define C_LINKED_DEQUE_TPP

#include "../include/c_linked_deque.h"

namespace Cloverwatch {

#define CLinkedNode CLinkedDeque<T, heap, standard_block_size>::Node

    template <typename T, Heap* heap, size_t standard_block_size>
    CLinkedNode* CLinkedDeque<T, heap, standard_block_size>::allocate_node(Direction dir) {

        constexpr size_t t_align = alignof(T);

        constexpr size_t aligned_header_size = (sizeof(Node) + t_align - 1) & ~(t_align - 1);

        size_t curr_block_size = standard_block_size;

        void* mem_block = nullptr;

        while (mem_block == nullptr && curr_block_size > 0) {
            size_t num_bytes = aligned_header_size + curr_block_size * sizeof(T);
            mem_block = heap->allocate<void>(num_bytes);
            curr_block_size >>= 1;
        }

        if (!mem_block) return nullptr;

        Node* node = new (mem_block) Node();
        node->size = (curr_block_size<<1);

        switch (dir) {
            case Direction::FRONT:
                node->front = 0;
                node->back = 0;
                break;
            case Direction::BACK:
                node->back = node->size;
                node->front = node->size;
                break;
        }

        node->next = nullptr;
        node->prev = nullptr;

        return node;
    }

    template <typename T, Heap* heap, size_t standard_block_size>
    bool CLinkedDeque<T, heap, standard_block_size>::push_back(T& val) {

        if (num_elements == 0) {
            front_node_ptr = allocate_node(Direction::BACK);
            if (!front_node_ptr) return false;
            back_node_ptr = front_node_ptr;
        }

        if (back_node_ptr->back == 0) {
            if (const auto res = add_back_node(); !res) return false;
        }

        auto& vals = back_node_ptr->vals;
        auto& back = back_node_ptr->back;

        vals[--back] = val;

        num_elements++;

        return true;
    }

    template <typename T, Heap* heap, size_t standard_block_size>
    bool CLinkedDeque<T, heap, standard_block_size>::pop_back(T& val) {
        if (num_elements == 0) return false;

        if (back_node_ptr->front == back_node_ptr->back) return false;

        auto& vals = back_node_ptr->vals;
        auto& back = back_node_ptr->back;
        auto& size = back_node_ptr->size;

        val = vals[back++];

        if (back == size || back == back_node_ptr->front) {
            remove_back_node();
        }

        num_elements--;

        return true;
    }

    template <typename T, Heap* heap, size_t standard_block_size>
    bool CLinkedDeque<T, heap, standard_block_size>::push_front(T& val) {

        if (num_elements == 0) {
            front_node_ptr = allocate_node(Direction::FRONT);

            if (!front_node_ptr) return false;

            back_node_ptr = front_node_ptr;
        }

        if (front_node_ptr->front == front_node_ptr->size) {
            if (const auto res = add_front_node(); !res) return false;
        }

        auto& front = front_node_ptr->front;
        auto& vals = front_node_ptr->vals;

        vals[front++] = val;

        num_elements++;

        return true;
    }

    template <typename T, Heap* heap, size_t standard_block_size>
    bool CLinkedDeque<T, heap, standard_block_size>::pop_front(T& val) {
        if (num_elements == 0) return false;

        if (front_node_ptr->front == front_node_ptr->back) return false;

        auto& vals = front_node_ptr->vals;
        auto& front = front_node_ptr->front;

        val = vals[--front];

        if (front == 0 || front == front_node_ptr->back) {
            remove_front_node();
        }

        num_elements--;

        return true;
    }

#undef CLinkedNode

}

#endif //C_LINKED_DEQUE_TPP
