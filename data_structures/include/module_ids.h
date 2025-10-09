//
// Created by saransh on 7/29/25.
//

#ifndef ENUM_IDS_H
#define ENUM_IDS_H

#include <cstdint>

namespace Cloverwatch {

    enum class ModuleId : uint8_t {
        LOGGER = 0,
        SERIAL_IO = 1,
        VALIDATION_MODULE = 2,
        TASK_MANAGER = 3,
        PATTERN_MATCHER = 4,
        CLI = 5,
        MAIN_THREAD = 6,
        CRC = 7,
        MAX
    };

    constexpr const char* ModuleId_to_string(ModuleId id) {

        switch (id) {
            case ModuleId::LOGGER:
                return "LOGGER";
            case ModuleId::SERIAL_IO:
                return "SERIAL_IO";
            case ModuleId::VALIDATION_MODULE:
                return "VALIDATION_MODULE";
            case ModuleId::TASK_MANAGER:
                return "TASK_MANAGER";
            case ModuleId::PATTERN_MATCHER:
                return "PATTERN_MATCHER";
            case ModuleId::CLI:
                return "CLI";
            case ModuleId::MAIN_THREAD:
                return "MAIN_THREAD";
            case ModuleId::CRC:
                return "CRC";
            default:
                return "UNDEFINED";
        };

    }

    
}

#endif //ENUM_IDS_H
