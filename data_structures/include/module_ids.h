//
// Created by saransh on 7/29/25.
//

#ifndef ENUM_IDS_H
#define ENUM_IDS_H

#include <cstdint>

namespace Cloverwatch {

    enum ModuleId {
        LOGGER = 0,
        SERIAL_IO = 1,
        VALIDATION_MODULE = 2,
        TASK_MANAGER = 3,
        PATTERN_MATCHER = 4,
        CLI = 5,
        MAIN_THREAD = 6,
        MAX
    };

    template <ModuleId id>
    constexpr const char* ModuleId_to_string() {

        if constexpr (id == LOGGER) {
            return "LOGGER";
        }
        else if constexpr (id == SERIAL_IO) {
            return "SERIAL_IO";
        }
        else if constexpr (id == VALIDATION_MODULE) {
            return "VALIDATION_MODULE";
        }
        else if constexpr (id == TASK_MANAGER) {
            return "TASK_MANAGER";
        }
        else if constexpr (id == PATTERN_MATCHER) {
            return "PATTERN_MATCHER";
        }
        else if constexpr (id == CLI) {
            return "CLI";
        }
        else if constexpr (id == MAIN_THREAD) {
            return "MAIN_THREAD";
        }
        else {
            return "INVALID_MODULE_ID";
        }
    }

    
}

#endif //ENUM_IDS_H
