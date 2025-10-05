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
        PacketBuffer packet_buffer;
        PacketBuffer transmit_buffer;

        auto& instance = This_Type::instance;

        instance.validation_queue.pop(ToWrite(buffer.as_vec()));

        auto user_data = This_Type::Instance().user_data;

        for (size_t i=0; i<buffer.len(); i++) {

            instance.validation_func(buffer[i], WriteBufferRef<void>(user_data), ToWrite(packet_buffer.as_vec()), ToWrite(transmit_buffer.as_vec()));

            if (transmit_buffer.len() > 0) {
                transmit(packet_buffer);
                transmit_buffer.clear();
            }

            if (packet_buffer.len() > 0) {
                bool res = instance.process_queue.push(std::move(packet_buffer));
                if (!res) {
                    Logger<ModuleId::SERIAL_IO>::log("Process queue full", LogLevel::WARNING);
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
                Logger<ModuleId::SERIAL_IO>::log("TX Done", LogLevel::DEBUG);
                break;
            }
            case UART_TX_ABORTED: {
                Logger<ModuleId::SERIAL_IO>::log("TX Aborted", LogLevel::DEBUG);
                break;
            }
            case UART_RX_RDY: {
                auto& [buf, offset, len] = evt->data.rx;

                Vector<Byte> buffer(buf + offset, len, len);

                if (! this_ptr->validation_queue.push(ToRead(buffer))) {
                    Logger<ModuleId::SERIAL_IO>::log("Validation queue full", LogLevel::WARNING);
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
                Logger<ModuleId::SERIAL_IO>::log("RX Buffer Released", LogLevel::DEBUG);
                break;
            }
            case UART_RX_DISABLED: {
                Logger<ModuleId::SERIAL_IO>::log("RX Disabled", LogLevel::DEBUG);
                break;
            }
            case UART_RX_STOPPED: {
                Logger<ModuleId::SERIAL_IO>::log("RX Stopped", LogLevel::DEBUG);
                break;
                }
        }
    }


    template <typename G, typename L>
    void Serial_DMAasync<G, L>::start_process(process_func validation_func, WriteBufferRef<void> user_data) {

        instance.validation_func = validation_func;
        instance.user_data = user_data.ref;

        uart_callback_set(L::dev, uart_callback, &instance);

        int res = uart_rx_enable(L::dev, buffer_pair[instance.current_buffer].data(), L::dma_buffer_size, 1000000);

        if (res) {
            Logger<ModuleId::SERIAL_IO>::log("Critical Error: UART not enabled", LogLevel::ERROR);
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

        int res = uart_tx(L::dev, bytes.data(), bytes.len(), K_NO_WAIT.ticks);

        if (res == 0) {
            Logger<ModuleId::SERIAL_IO>::log("TX Success", LogLevel::DEBUG);
        }
        else {
            Logger<ModuleId::SERIAL_IO>::log("TX Failed", LogLevel::DEBUG);
        }
    }

}

#include "../compile_time_init/serial_template_config.cpp"