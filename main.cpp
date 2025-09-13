#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/devicetree.h>

#include "serial_io.h"
#include "task_manager.h"
#include "data_structures/include/module_ids.h"

#include "Validator/include/simple_packetiser.h"
#include "Validator/include/reed_soloman.h"

#include "PatternMatcher/include/pattern.h"
#include "PatternMatcher/include/pattern_stats.h"

namespace Cloverwatch {

    using Serial_IO_Wire = Serial_DMAasync<DEVICE_DT_GET(DT_NODELABEL(uart1)), 1024, 1024, 4>;

    using MainValidator = SimplePacketiser_Block<2048, RS_Validator::encode<127, 4>, RS_Validator::decode<127, 4>>;

    MainValidator validator({
        .header_byte = 0xff,
        .header_size = 2,

        .footer_byte = 0x11,
        .footer_size = 2,

        .escape_byte = 0x22,
        .length_size = 2
    });

    void validation_func(const Byte byte, WriteBufferPtr<void> user_data, WriteVector<Byte> transmit_data, WriteVector<Byte> completed_packet) {

        auto validator = static_cast<MainValidator*>(user_data.ptr);

        validator->add_byte(byte, completed_packet);

        if (completed_packet.len > 0) {
            transmit_data.clear();
            validator->construct_packet(completed_packet.to_read(), transmit_data);
        }
    }

    void serial_IOWIRE_startup() {
        Serial_IO_Wire::Instance().start_process(validation_func, WriteBufferPtr<void>(&validator));

        if (!device_is_ready(DEVICE_DT_GET(DT_NODELABEL(uart1)))) {
            Logger<ModuleId::MAIN_THREAD>::log(ReadPtr<char>("UART not ready"), LogLevel::ERROR);
            exit(1);
            //TODO: Replace exit with better alternative
        }

    }


    void start() {

        TaskManager::Instance().init();
        serial_IOWIRE_startup();

        Logger<ModuleId::MAIN_THREAD>::log(ReadPtr<char>("Starting"), LogLevel::INFO);

        while (true) {
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