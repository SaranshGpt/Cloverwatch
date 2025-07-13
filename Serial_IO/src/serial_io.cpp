//
// Created by saransh on 6/14/25.
//

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include "serial_io.h"

#include "task_manager.h"

namespace Cloverwatch {

    template <const device* dev, uint16_t buffer_size, uint16_t num_buffers>
    Serial_DMAasync<dev, buffer_size, num_buffers> Serial_DMAasync<dev, buffer_size, num_buffers>::instance;

    template <const device* dev, uint16_t buffer_size, uint16_t num_buffers>
    void Serial_DMAasync<dev, buffer_size, num_buffers>::validation_handler(void* args) {

        while (!instance.validation_queue.empty()) {
            auto ret = instance.validation_queue.pop();
            if (!ret.has_value()) break;

            auto & buffer_ret = ret.value();

            if (instance.validation_func(buffer_ret, instance.user_data)) {
                uart_tx(dev, buffer_ret.buffer.data(), buffer_ret.size, SYS_FOREVER_US);
                
                instance.process_queue.push(std::move(buffer_ret));
            }
            else {
                //TODO: Add logging when implemented
            }

        }

    }

    template<const device *dev, uint16_t buffer_size, uint16_t num_buffers>
    void Serial_DMAasync<dev, buffer_size, num_buffers>::uart_callback(const struct device *dev_ptr, struct uart_event *evt, void* user_data) {

    auto* this_ptr = static_cast<This_Type*>(user_data);

    switch (evt->type) {
        case UART_TX_DONE: {
            //TODO: Add logging when implemented
            break;
        }
        case UART_TX_ABORTED: {
            //TODO: Add logging when implemented
            break;
        }
        case UART_RX_RDY: {
            auto& [buf, offset, len] = evt->data.rx;

            Buffer<buffer_size> buffer;

            if (len > 0) {
                memcpy(buffer.buffer.data(), buf + offset, len);
            }

            buffer.size = len;

            if (! this_ptr->validation_queue.push(std::move(buffer))) {
                //TODO: Add logging when implemented
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
            //TODO: Add logging if needed. The case is largely redundant.
            break;
        }
        case UART_RX_DISABLED: {
            //TODO: Add logging when implemented
            break;
        }
        case UART_RX_STOPPED: {
            //TODO: Add logging when implemented
            break;
        }
    }
}


    template <const device* dev, uint16_t buffer_size, uint16_t num_buffers>
    void Serial_DMAasync<dev, buffer_size, num_buffers>::start_process(process_func validation_func, void* user_data) {

        instance.validation_func = validation_func;
        instance.user_data = user_data;

        uart_callback_set(dev, uart_callback, this);

        uart_rx_enable(dev, buffer_pair[instance.current_buffer].data(), buffer_size);

        instance.current_buffer = !instance.current_buffer;

        const TaskManager::Task task = {validation_handler, nullptr, TaskManager::Priority::URGENT};

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

    template <const device* dev, uint16_t buffer_size, uint16_t num_buffers>
    void Serial_DMAasync<dev, buffer_size, num_buffers>::stop_process() {
        //Will be implemented later. Not a priority right now
    }

    template <const device* dev, uint16_t buffer_size, uint16_t num_buffers>
    std::optional<Buffer<buffer_size>> Serial_DMAasync<dev, buffer_size, num_buffers>::pop() {
        return process_queue.pop();
    }

}