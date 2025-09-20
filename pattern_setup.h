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

    inline void process_func(void* args) {



    }

}

#endif //CLOVERWATCH_PATTERN_SETUP_H