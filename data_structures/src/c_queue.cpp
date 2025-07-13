//
// Created by saransh on 6/15/25.
//


#include "c_queue.h"

namespace Cloverwatch {

    template<typename T, uint16_t size>
    int CQueue_concurrent<T, size>::acquire_front_position() {

        int curr_front;
        int next_front;

        do {
            curr_front = front.load();
            if (curr_front == back.load()) return -1;
            next_front = curr_front + 1 - size*(curr_front+1==size);
        } while (
            !front.compare_exchange_weak(curr_front, next_front, std::memory_order_acquire, std::memory_order_relaxed)
        );

        return curr_front;
    }

    template<typename T, uint16_t size>
    int CQueue_concurrent<T, size>::acquire_back_position() {

        int curr_back = back.load();
        int next_back = curr_back + 1 - size*(curr_back+1==size);

        if (curr_back == front.load()-1) return -1;

        back.store(next_back, std::memory_order_release);

        return curr_back;
    }

    template <typename T, uint16_t size>
    bool CQueue_concurrent<T, size>::push(T&& t)
    {
        int back_pos = acquire_back_position();
        if(back_pos == -1) return false;

        queue[back_pos] = std::move(t);
        return true;
    }

    template<typename T, uint16_t size>
    std::optional<T> CQueue_concurrent<T, size>::pop() {

        int front_pos = acquire_front_position();
        if(front_pos == -1) return std::nullopt;

        return std::move(queue[front_pos]);
    }

}