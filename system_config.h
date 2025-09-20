//
// Created by saransh on 9/18/25.
//

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "TaskManager/include/task_manager.h"
#include "data_structures/include/c_types.h"

#include <zephyr/device.h>

#include <zephyr/logging/log.h>

#define CONFIG(type) static constexpr const type

namespace Cloverwatch {

    struct ThreadConfig {
        TaskManager::Priority priority;
    };

    struct GlobalConfig {
        CONFIG(size_t) max_packet_size = 2 * 1024;
    };

    struct SerialIOConfig {
        CONFIG(device*) dev = DEVICE_DT_GET(DT_NODELABEL(uart1));

        CONFIG(size_t) baud_rate = 115200;
        CONFIG(size_t) dma_buffer_size = 1024;

        CONFIG(size_t) num_packet_buffers = 4;

        CONFIG(ThreadConfig) thread = {TaskManager::Priority::URGENT};
    };

    struct PatternConfig {
        CONFIG(size_t) max_stack_size = 20;
        CONFIG(size_t) max_patterns = 10;
        CONFIG(size_t) heap_size = 20 * 1024;
        CONFIG(size_t) history_length = 10;

        CONFIG(size_t) max_byte_processed = 50;

        CONFIG(ThreadConfig) thread = {TaskManager::Priority::MEDIUM};
    };

    struct ValidatorConfig {

        enum class Endianness {
            LITTLE,
            BIG
        };

        struct SimplePacketiser_Block {

            CONFIG(Byte) header_byte = 0xff;
            CONFIG(Byte) header_size = 2;

            CONFIG(Byte) footer_byte = 0x11;
            CONFIG(Byte) footer_size = 2;

            CONFIG(Byte) escape_byte = 0x22;
            CONFIG(Byte) length_size = 2;

            CONFIG(Endianness) endianness = Endianness::LITTLE;
        };

        struct RS {
            CONFIG(uint8_t) chunk_size = 127;
            CONFIG(uint8_t) max_symbols = 4;
        };
    };

}

#undef CONFIG

#endif //SYSTEM_CONFIG_H
