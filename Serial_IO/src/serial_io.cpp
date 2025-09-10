//
// Created by saransh on 6/14/25.
//

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include "serial_io.h"

#include "task_manager.h"

namespace Cloverwatch {

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers> Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::instance;

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::validation_handler(void* args) {

        FixedBuffer<buffer_size> buffer;

        auto& instance = This_Type::instance;

        instance.validation_queue.pop(buffer.to_write());

        auto user_data = This_Type::Instance().user_data;

        FixedBuffer<packet_size> packet_buffer;
        FixedBuffer<packet_size> transmit_buffer;

        for (size_t i=0; i<buffer.len; i++) {

            instance.validation_func(buffer[i], user_data, packet_buffer.to_write(), transmit_buffer.to_write());

            if (transmit_buffer.len > 0) {
                transmit(packet_buffer);
                transmit_buffer.clear();
            }

            if (packet_buffer.len > 0) {
                bool res = instance.process_queue.push(std::move(packet_buffer));
                if (!res) {
                    Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("Process queue full"), LogLevel::WARNING);
                }
                packet_buffer.clear();
            }

        }
    }

    template<const device *dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::uart_callback(const device *dev_ptr, uart_event *evt, void* user_data) {

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
                uart_rx_buf_rsp(dev, buffer, buffer_size);
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


    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::start_process(process_func validation_func, WriteBufferPtr<void> user_data) {

        instance.validation_func = validation_func;
        instance.user_data = user_data;

        uart_callback_set(dev, uart_callback, &instance);

        int res = uart_rx_enable(dev, buffer_pair[instance.current_buffer].data(), buffer_size, 1000000);

        if (res) {
            Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("Critical Error: UART not enabled"), LogLevel::ERROR);
        }

        instance.current_buffer = !instance.current_buffer;

        constexpr TaskManager::Task task = {validation_handler, nullptr, TaskManager::Priority::URGENT};

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::stop_process() {
        //TODO: Will be implemented later. Not a priority right now
    }

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    std::optional<FixedBuffer<packet_size>> Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::pop() {
        return process_queue.pop();
    }

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::transmit(FixedBuffer<packet_size>& bytes) {

        int res = uart_tx(dev, bytes.buffer.data(), bytes.len, K_NO_WAIT.ticks);

        if (res == 0) {
            Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("TX Success"), LogLevel::DEBUG);
        }
        else {
            Logger<ModuleId::SERIAL_IO>::log(ReadPtr<char>("TX Failed"), LogLevel::DEBUG);
        }
    }

}

#include "../compile_time_init/serial_template_config.cpp"