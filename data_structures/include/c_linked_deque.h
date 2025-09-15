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
            size_t front;
            size_t back;
            size_t size;
            Node* next = nullptr;
            Node* prev = nullptr;
            T vals[];
        };

        enum class Direction {
            FRONT,
            BACK
        };

        Node* front_node_ptr = nullptr;
        Node* back_node_ptr = nullptr;

        size_t num_elements = 0;

        static Node* allocate_node(Direction dir);

        bool add_front_node() {
            auto new_front = allocate_node(Direction::FRONT);

            if (new_front == nullptr) return false;

            front_node_ptr->next = new_front;
            new_front->prev = front_node_ptr;
            front_node_ptr = new_front;

            return true;
        }

        bool add_back_node() {
            auto new_back = allocate_node(Direction::BACK);

            if (new_back == nullptr) return false;

            back_node_ptr->prev = new_back;
            new_back->next = back_node_ptr;
            back_node_ptr = new_back;

            return true;
        }
        void remove_front_node() {
            auto new_front = front_node_ptr->prev;
            heap->free(front_node_ptr);
            front_node_ptr = new_front;

            if (front_node_ptr == nullptr) {
                back_node_ptr = nullptr;
            }
            else {
                front_node_ptr->next = nullptr;
            }
        }
        void remove_back_node() {
            auto new_back = back_node_ptr->next;
            heap->free(back_node_ptr);
            back_node_ptr = new_back;

            if (back_node_ptr == nullptr) {
                front_node_ptr = nullptr;
            }
            else {
                back_node_ptr->prev = nullptr;
            }
        }

    public:

        bool push_back(T& val);
        bool pop_back(T& val);

        bool push_front(T& val);
        bool pop_front(T& val);

        T& peek_front() {
            return front_node_ptr->vals[front_node_ptr->front-1];
        }

        T& peek_back() {
            return back_node_ptr->vals[back_node_ptr->back];
        }

        bool empty() {
            return (front_node_ptr == nullptr);
        }

        size_t size() {
            return num_elements;
        }
    };

}

#include "../src/c_linked_deque.tpp"

#endif //C_LINKED_VECTOR_H
