#ifndef CQUEUE_H
#define CQUEUE_H

#include <array>
#include <memory>
#include <atomic>
#include <optional>

namespace Cloverwatch {

    template <typename T, uint16_t size>
    class CQueue_concurrent {
    public:

        bool push(T&& t);
        std::optional<T> pop();

        // Fixed: atomic-safe methods
        bool empty() const { 
            return front.load(std::memory_order_acquire) == 
                   back.load(std::memory_order_acquire); 
        }
        
        int capacity_remaining() const {
            auto f = front.load(std::memory_order_acquire);
            auto b = back.load(std::memory_order_acquire);
            return (size - 1) - ((b - f + size) % size);
        }

    private:

        std::atomic<int> front{0};
        std::atomic<int> back{0};
        std::array<T, size> queue;

        int acquire_front_position();
        int acquire_back_position();

    };

}

#endif //CQUEUE_H