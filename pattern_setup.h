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
        auto &serial = Serial_IO_Wire::Instance();

        if (auto packet = serial.pop(); packet.has_value()) {
            PrimaryPatternConfig::object.add_packet(ToRead(packet.value().as_vec()));
        }
    }

    inline void pattern_setup() {

        PrimaryPatternConfig::object.start_process();

        TaskManager::Task task = {
            .func = pattern_process_func,
            .args = nullptr,
            .priority = TaskManager::Priority::MEDIUM
        };

        TaskManager::Instance().forever_work_item(task, K_MSEC(1));
    }

}

#endif //CLOVERWATCH_PATTERN_SETUP_H