#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>

#include "serial_io.h"
#include "task_manager.h"
#include "data_structures/include/module_ids.h"

namespace Cloverwatch {

    using Serial_IO_Wire = Serial_DMAasync<DEVICE_DT_GET(DT_NODELABEL(uart1)), 1024, 1024, 4>;

    std::optional<Buffer<1024>> validation_func(ReadVector<Byte> bytes, WritePtr<void> user_data) {
        return Buffer<1024>();
    }

    void serial_IOWIRE_startup() {
        Serial_IO_Wire::Instance().start_process(validation_func, nullptr);

        if (!device_is_ready(DEVICE_DT_GET(DT_NODELABEL(uart1)))) {
            Logger<ModuleId::MAIN_THREAD>::log(ReadPtr<char>("UART not ready"), LogLevel::ERROR);
            return;
        }

    }


    void start() {

        TaskManager::Instance().init();
        serial_IOWIRE_startup();

        Logger<ModuleId::MAIN_THREAD>::log(ReadPtr<char>("Starting"), LogLevel::INFO);

        while (1) {
            k_sleep(K_MSEC(1000));
        }
    }


}

extern "C" {

int main(void) {

    Cloverwatch::start();

    return 0;
}

}