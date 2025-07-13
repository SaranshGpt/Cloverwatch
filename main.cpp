#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>

#include "serial_io.h"

namespace Cloverwatch {

    using Serial_IO_Wire = Serial_DMAasync<DEVICE_DT_GET(DT_NODELABEL(uart0)), 1024, 1024>;

    bool validation_func(Buffer<1024>& buffer, void* user_data) {
        printk("Recieved\n");
        return true;
    }

    int main() {

        Serial_IO_Wire::Instance().start_process(validation_func, nullptr);

        while (1) {
            k_sleep(K_MSEC(1000));
        }

        return 0;
    }
}