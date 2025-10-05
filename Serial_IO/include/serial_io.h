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

    template <typename G, typename L>
    class Serial_DMAasync {

        using PacketBuffer = FixedBuffer<G::max_packet_size>;

        using This_Type = Serial_DMAasync<G, L>;

    public:

        static Serial_DMAasync& Instance() { return instance; }

        using process_func = void (*)(Byte byte, WriteBufferRef<void> user_data, WriteVector<Byte> transmit_data, WriteVector<Byte> completed_packet);

        void start_process(process_func validation_func, WriteBufferRef<void> user_data);
        void stop_process();

        std::optional<PacketBuffer> pop();

    private:

        static Serial_DMAasync instance;

        CByteQueue_SPSC<L::dma_buffer_size> validation_queue;
        CQueue_concurrent_SPSC<PacketBuffer, L::num_packet_buffers> process_queue;

        process_func validation_func = nullptr;
        void* user_data = nullptr;

        std::array<std::array<uint8_t, L::dma_buffer_size>, 2> buffer_pair;
        bool current_buffer = 0;

        static void uart_callback(const struct device *dev_ptr, struct uart_event *evt, void* user_data);
        static void transmit(PacketBuffer& bytes);

        static void validation_handler(void* args);

        Serial_DMAasync() {};

    };

}

#endif //SERIAL_IO_H
