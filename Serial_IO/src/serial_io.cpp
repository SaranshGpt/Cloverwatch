//
// Created by saransh on 6/14/25.
//

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include "serial_io.h"

#include "task_manager.h"
#include "../../system_config.h"

namespace Cloverwatch {

    template <typename G, typename L>
    Serial_DMAasync<G, L> Serial_DMAasync<G, L>::instance;

    template <typename G, typename L>
    void Serial_DMAasync<G, L>::validation_handler(void* args) {

        PacketBuffer buffer;

        auto& instance = This_Type::instance;

        instance.validation_queue.pop(buffer.to_write());

        auto user_data = This_Type::Instance().user_data;

        PacketBuffer packet_buffer;
        PacketBuffer transmit_buffer;

        for (size_t i=0; i<buffer.size(); i++) {

            instance.validation_func(buffer[i], user_data, packet_buffer.to_write(), transmit_buffer.to_write());

            if (transmit_buffer.size() > 0) {
                transmit(packet_buffer);
                transmit_buffer.clear();
            }

            if (packet_buffer.size() > 0) {
                bool res = instance.process_queue.push(std::move(packet_buffer));
                if (!res) {
                    Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("Process queue full"), LogLevel::WARNING);
                }
                packet_buffer.clear();
            }

        }
    }

    template <typename G, typename L>
    void Serial_DMAasync<G, L>::uart_callback(const device *dev_ptr, uart_event *evt, void* user_data) {

        (void)dev_ptr;

        auto* this_ptr = static_cast<This_Type*>(user_data);

        switch (evt->type) {
            case UART_TX_DONE: {
                Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("TX Done"), LogLevel::DEBUG);
                break;
            }
            case UART_TX_ABORTED: {
                Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("TX Aborted"), LogLevel::DEBUG);
                break;
            }
            case UART_RX_RDY: {
                auto& [buf, offset, len] = evt->data.rx;

                Vector<Byte> buffer(buf + offset, len, len);

                if (! this_ptr->validation_queue.push(buffer.to_read())) {
                    Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("Validation queue full"), LogLevel::WARNING);
                }
                break;
            }
            case UART_RX_BUF_REQUEST: {
                uint8_t* buffer = this_ptr->buffer_pair[this_ptr->current_buffer].data();
                this_ptr->current_buffer = !this_ptr->current_buffer;
                uart_rx_buf_rsp(L::dev, buffer, L::dma_buffer_size);
                break;
            }
            case UART_RX_BUF_RELEASED: {
                Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("RX Buffer Released"), LogLevel::DEBUG);
                break;
            }
            case UART_RX_DISABLED: {
                Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("RX Disabled"), LogLevel::DEBUG);
                break;
            }
            case UART_RX_STOPPED: {
                Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("RX Stopped"), LogLevel::DEBUG);
                break;
                }
        }
    }


    template <typename G, typename L>
    void Serial_DMAasync<G, L>::start_process(process_func validation_func, WriteBufferPtr<void> user_data) {

        instance.validation_func = validation_func;
        instance.user_data = user_data;

        uart_callback_set(L::dev, uart_callback, &instance);

        int res = uart_rx_enable(L::dev, buffer_pair[instance.current_buffer].data(), L::dma_buffer_size, 1000000);

        if (res) {
            Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("Critical Error: UART not enabled"), LogLevel::ERROR);
        }

        instance.current_buffer = !instance.current_buffer;

        constexpr TaskManager::Task task = {validation_handler, nullptr, TaskManager::Priority::URGENT};

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

    template <typename G, typename L>
    void Serial_DMAasync<G, L>::stop_process() {
        //TODO: Will be implemented later. Not a priority right now
    }

    template <typename G, typename L>
    std::optional<FixedBuffer<G::max_packet_size>> Serial_DMAasync<G, L>::pop() {
        return process_queue.pop();
    }

    template <typename G, typename L>
    void Serial_DMAasync<G, L>::transmit(PacketBuffer& bytes) {

        int res = uart_tx(L::dev, bytes.data(), bytes.size(), K_NO_WAIT.ticks);

        if (res == 0) {
            Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("TX Success"), LogLevel::DEBUG);
        }
        else {
            Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("TX Failed"), LogLevel::DEBUG);
        }
    }

}

#include "../compile_time_init/serial_template_config.cpp"