//
// Created by saransh on 6/14/25.
//

#ifndef SERIAL_IO_H
#define SERIAL_IO_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include <array>
#include <atomic>

#include "../../data_structures/include/c_queue.h"
#include "zephyr/net_buf.h"

namespace Cloverwatch {

    template <uint16_t buffer_size>
    struct Buffer {
        std::array<uint8_t, buffer_size> buffer;
        uint16_t size = 0;
    };

    template <const device* dev, uint16_t buffer_size, uint16_t num_buffers>
    class Serial_DMAasync {

    public:

        static_assert(buffer_size > 0, "Invalid buffer size");
        static_assert(num_buffers > 0, "Invalid value of num_buffers");

        using This_Type = Serial_DMAasync<dev, buffer_size, num_buffers>;

        static Serial_DMAasync& Instance() { return instance; }

        using process_func = bool (*)(Buffer<buffer_size>& buffer, void* user_data);

        void start_process(process_func validation_func, void* user_data);
        void stop_process();

        std::optional<Buffer<buffer_size>> pop();

    private:

        static Serial_DMAasync instance;

        CQueue_concurrent<Buffer<buffer_size>, num_buffers> validation_queue;
        CQueue_concurrent<Buffer<buffer_size>, 2*num_buffers> process_queue;

        process_func validation_func = nullptr;
        void* user_data = nullptr;

        std::array<std::array<uint8_t, buffer_size>, 2> buffer_pair;
        bool current_buffer = 0;

        static void uart_callback(const struct device *dev_ptr, struct uart_event *evt, void* user_data);

        static void validation_handler(void* args);

        Serial_DMAasync() {};

    };

}

#endif //SERIAL_IO_H
