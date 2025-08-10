#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>

#include "serial_io.h"
#include "task_manager.h"
#include "data_structures/include/module_ids.h"

#include "Validator/include/validator.h"
#include "Validator/include/reed_soloman.h"

namespace Cloverwatch {

    using Serial_IO_Wire = Serial_DMAasync<DEVICE_DT_GET(DT_NODELABEL(uart1)), 1024, 1024, 4>;

    using MainValidator = BlockValidator<2048, RS_Validator::decode<255, 4>>;

    void validation_func(ReadVector<Byte> bytes, WritePtr<void> user_data, WriteVector<Byte> transmit_data, WriteVector<Byte> completed_packet) {

        auto validator = static_cast<MainValidator>(user_data.ptr());

        validator.add_bytes(bytes, transmit_data);

        if (transmit_data.len > 0)
            for (int i=0; i<transmit_data.len; i++) {
                completed_packet[i] = transmit_data[i];
            }
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