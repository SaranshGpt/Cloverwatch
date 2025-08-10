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
#include "../../data_structures/include/ptr_types.h"
#include "../../data_structures/include/c_types.h"

#include "zephyr/net_buf.h"

#include "../../Logger/include/logger.h"

namespace Cloverwatch {

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    class Serial_DMAasync {

    public:

        static_assert(buffer_size > 0, "Invalid buffer size");
        static_assert(packet_size > 0, "Invalid value of packet_size");
        static_assert(num_packet_buffers > 0, "Invalid value of num_packet_buffers");

        using This_Type = Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>;

        static Serial_DMAasync& Instance() { return instance; }

        using process_func = void (*)(ReadVector<Byte> bytes, WritePtr<void> user_data, WriteVector<Byte> transmit_data, WriteVector<Byte> completed_packet);

        void start_process(process_func validation_func, void* user_data);
        void stop_process();

        std::optional<Buffer<packet_size>> pop();

    private:

        static Serial_DMAasync instance;


        CByteQueue_SPSC<buffer_size> validation_queue;
        CQueue_concurrent_SPSC<Buffer<packet_size>, num_packet_buffers> process_queue;

        process_func validation_func = nullptr;
        void* user_data = nullptr;

        std::array<std::array<uint8_t, buffer_size>, 2> buffer_pair;
        bool current_buffer = 0;

        static void uart_callback(const struct device *dev_ptr, struct uart_event *evt, void* user_data);
        static void transmit(Buffer<packet_size>& bytes);

        static void validation_handler(void* args);

        Serial_DMAasync() {};

    };

}

#endif //SERIAL_IO_H
