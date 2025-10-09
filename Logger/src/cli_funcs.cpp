//
// Created by saransh on 7/23/25.
//

#include "zephyr/logging/log.h"

#include "cli_funcs.h"
#include "logger.h"

#include "../../Config/system_config.h"

namespace Cloverwatch {

    template class Logger<GlobalConfig, LoggerConfig>;

    Cli::CommandRes display_logs(Cli::Shell sh) {

        FixedVector<LogEntry, LoggerConfig::max_entries> log_store;

        auto& logger = Objects::logger;

        logger.get_entries(ToWrite(log_store));

        for (auto& entry: log_store) {
            auto module_name = ModuleId_to_string(entry.module);

            switch (entry.level) {
                case LogLevel::INFO:
                    sh.print<Cli::Shell::Level::NORMAL>("<%s:%d> %s\n", module_name, entry.timestamp.seconds(), entry.message);
                    break;
                case LogLevel::WARNING:
                    sh.print<Cli::Shell::Level::WARN>("<%s:%d> %s\n", module_name, entry.timestamp.seconds(), entry.message);
                    break;
                case LogLevel::ERROR:
                    sh.print<Cli::Shell::Level::ERROR>("<%s:%d> %s\n", module_name, entry.timestamp.seconds(), entry.message);
                    break;
                case LogLevel::DEBUG:
                    sh.print<Cli::Shell::Level::INFO>("<%s:%d> %s\n", module_name, entry.timestamp.seconds(), entry.message);
                    break;
                default:
                    sh.print<Cli::Shell::Level::NORMAL>("<%s:%d> %s\n", module_name, entry.timestamp.seconds(), entry.message);
            }
        }

        return Cli::CommandRes::SUCCESS;
    }


}