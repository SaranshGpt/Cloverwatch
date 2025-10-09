//
// Created by saransh on 9/20/25.
//

#ifndef CLOVERWATCH_PATTERN_SETUP_H
#define CLOVERWATCH_PATTERN_SETUP_H

#include <zephyr/kernel.h>

#include "serial_io.h"
#include "task_manager.h"
#include "PatternMatcher/include/pattern_stats.h"

#include "Config/system_config.h"
#include "serial_setup.h"

namespace Cloverwatch {

    inline void pattern_process_func(void* args) {
        auto &serial = Objects::Serial_IO_Wire::Instance();

        if (auto packet = serial.pop(); packet.has_value()) {

            auto& val = packet.value().as_vec();

            Objects::primary_stats.add_packet(ToRead(val));

        }
    }

    inline void pattern_setup() {

        TaskManager::Task task = {
            .func = pattern_process_func,
            .args = nullptr,
            .priority = TaskManager::Priority::MEDIUM
        };

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

}

#endif //CLOVERWATCH_PATTERN_SETUP_H