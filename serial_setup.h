//
// Created by saransh on 9/20/25.
//

#ifndef CLOVERWATCH_SETUP_H
#define CLOVERWATCH_SETUP_H

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

#include "Config/system_config.h"

#include "CLI/include/cli.h"

namespace Cloverwatch {

    using RS_Funcs = RS_Validator<ValidatorConfig::RS::chunk_size, ValidatorConfig::RS::max_symbols>;

    using MainValidator = SimplePacketiser_Block<GlobalConfig, ValidatorConfig::SimplePacketiser_Block, RS_Funcs::encode, RS_Funcs::decode>;

    inline MainValidator main_validator;

    inline void validation_func(const Byte byte, WriteBufferRef<void> user_data, WriteVector<Byte> transmit_data, WriteVector<Byte> completed_packet) {

        const auto validator = static_cast<MainValidator*>(user_data.ref);

        validator->add_byte(byte, completed_packet);

        if (completed_packet.ref.len() > 0) {
            transmit_data.ref.clear();
            validator->construct_packet(ToRead(completed_packet.ref), transmit_data);
        }
    }


    inline void serial_io_wire_startup() {

        Objects::Serial_IO_Wire::Instance().start_process(validation_func, WriteBufferRef<void>((&main_validator)));

        if (!device_is_ready(DEVICE_DT_GET(DT_NODELABEL(uart1)))) {
            Logger<ModuleId::MAIN_THREAD>::log("UART not ready", LogLevel::ERROR);
            exit(1);
            //TODO: Replace exit with better alternative
        }

    }

}

#endif //CLOVERWATCH_SETUP_H