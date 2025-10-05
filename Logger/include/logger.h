//
// Created by saransh on 7/23/25.
//

#ifndef LOGGER_H
#define LOGGER_H

#include "../logger_config.h"
#include "../../build/zephyr/misc/generated/syscalls_links/include_zephyr/logging/log_instance.h"
#include "../../data_structures/include/c_queue.h"

#include "../../data_structures/include/ptr_types.h"
#include "../../data_structures/include/c_types.h"
#include "../../data_structures/include/module_ids.h"

#include <zephyr/kernel.h>

#include "../../build/zephyr/misc/generated/syscalls_links/include_zephyr_logging/log.h"


namespace Cloverwatch {

    LOG_MODULE_DECLARE(Cloverwatch);

    enum class LogLevel {
        INFO,
        DEBUG,
        WARNING,
        ERROR
    };

    template <ModuleId module>
    class Logger {

        static std::array<char, LoggerConfig::LOG_BUFFER_SIZE> buffer;

    public:

        static void log(const char* message, LogLevel level = LogLevel::INFO) {

            switch (level) {
                case LogLevel::INFO:
                    LOG_INF("%s%s%s", ModuleId_to_string<module>(), ": ", message);
                    break;
                case LogLevel::DEBUG:
                    LOG_DBG("%s%s%s", ModuleId_to_string<module>(), ": ", message);
                    break;
                case LogLevel::WARNING:
                    LOG_WRN("%s%s%s", ModuleId_to_string<module>(), ": ", message);
                    break;
                case LogLevel::ERROR:
                    LOG_ERR("%s%s%s", ModuleId_to_string<module>(), ": ", message);
                    break;
                default:
                    log("Invalid log level", LogLevel::WARNING);
            }
        }

    };

}

#endif //LOGGER_H
