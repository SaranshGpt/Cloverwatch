//
// Created by saransh on 9/20/25.
//

#ifndef CLOVERWATCH_PATTERN_SETUP_H
#define CLOVERWATCH_PATTERN_SETUP_H

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

#include "system_config.h"
#include "serial_setup.h"

namespace Cloverwatch {

    using PatternStats = Pattern::StatTracker<GlobalConfig, PatternConfig>;

    static auto pattern_stats = PatternStats();

    inline void pattern_process_func(void* args) {
        auto &serial = Serial_IO_Wire::Instance();

        if (auto packet = serial.pop(); packet.has_value()) {
            pattern_stats.add_packet(packet.value().to_read());
        }
    }

    inline void pattern_setup() {
        pattern_stats.start_process();

        TaskManager::Task task = {
            .func = pattern_process_func,
            .args = nullptr,
            .priority = TaskManager::Priority::MEDIUM
        };

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

}

#endif //CLOVERWATCH_PATTERN_SETUP_H