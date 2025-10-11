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

        Node* reserved_nodes = nullptr;

        size_t num_elements = 0;
        size_t reserved_capacity = 0;

        static Node* allocate_node(Direction dir);

        bool add_front_node() {
            Node* new_front;

            if (reserved_nodes == nullptr) {
                new_front = allocate_node(Direction::FRONT);
            }
            else {
                new_front = reserved_nodes;

                *new_front = {0, 0, new_front->size, nullptr, nullptr};

                reserved_capacity -= new_front->size;
            }

            if (new_front == nullptr) return false;

            front_node_ptr->next = new_front;
            new_front->prev = front_node_ptr;
            front_node_ptr = new_front;

            return true;
        }

        bool add_back_node() {
            Node* new_back;

            if (reserved_nodes == nullptr) {
                new_back = allocate_node(Direction::BACK);
            }
            else {
                new_back = reserved_nodes;
                reserved_nodes = reserved_nodes->prev;

                *new_back = {new_back->size, new_back->size, new_back->size, nullptr, nullptr};
                reserved_capacity -= new_back->size;
            }

            if (new_back == nullptr) return false;

            back_node_ptr->prev = new_back;
            new_back->next = back_node_ptr;
            back_node_ptr = new_back;

            return true;
        }
        void remove_front_node() {
            auto new_front = front_node_ptr->prev;

            front_node_ptr->next = reserved_nodes;
            reserved_nodes = front_node_ptr;
            reserved_capacity += reserved_nodes->size;

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

            back_node_ptr->next = reserved_nodes;
            reserved_nodes = back_node_ptr;
            reserved_capacity -= reserved_nodes->size;

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

        bool reserve(size_t num_elements) {

            size_t curr_size = 0;
            bool res = true;

            while (res && curr_size < num_elements) {
                auto new_head = allocate_node(Direction::FRONT);

                if (new_head == nullptr) {
                    res = false;
                    break;
                }

                new_head->next = reserved_nodes;
                reserved_nodes = new_head;
                curr_size += new_head->size;
            }

            if (!res) {
                while (curr_size > 0) {
                    curr_size -= reserved_nodes->size;
                    reserved_nodes = reserved_nodes->next;
                    heap->free(reserved_nodes);
                }
            }

            reserved_capacity += num_elements;

            return res;
        }

        size_t capacity() {
            return reserved_capacity;
        }
    };

}

#include "../src/c_linked_deque.tpp"

#endif //C_LINKED_VECTOR_H
