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

        Buffer<buffer_size> buffer;

        This_Type::Instance().validation_queue.pop(
            WriteVector<Byte>(
                buffer.buffer.data(),
                buffer_size
            ));

        auto user_data = This_Type::Instance().user_data;

        Buffer<packet_size> packet_buffer;
        Buffer<packet_size> transmit_buffer;

        This_Type::Instance().validation_func(buffer.to_ReadVector(), user_data, packet_buffer.to_WriteVector(), transmit_buffer.to_WriteVector());

        if (transmit_buffer.size > 0)
            transmit(packet_buffer);

        if (packet_buffer.size > 0)
            process_queue.push(packet_buffer);

    }

    template<const device *dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::uart_callback(const struct device *dev_ptr, struct uart_event *evt, void* user_data) {

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


            if (! this_ptr->validation_queue.push(ReadVector<Byte>(
                ReadPtr<Byte>(reinterpret_cast<const Byte*>(buf + offset)), len, len
            ))) {
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
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::start_process(process_func validation_func, void* user_data) {

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
        //Will be implemented later. Not a priority right now
    }

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    std::optional<Buffer<packet_size>> Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::pop() {
        return process_queue.pop();
    }

    template <const device* dev, uint16_t buffer_size, uint16_t packet_size, uint8_t num_packet_buffers>
    void Serial_DMAasync<dev, buffer_size, packet_size, num_packet_buffers>::transmit(Buffer<packet_size>& bytes) {

        uart_tx(dev, bytes.buffer.data(), bytes.size, K_NO_WAIT);

    }

}

#include "../compile_time_init/serial_template_config.cpp"