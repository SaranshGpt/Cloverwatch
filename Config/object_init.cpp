//
// Created by saransh on 10/6/25.
//

#include "system_config.h"

namespace Cloverwatch {

    Pattern::StatTracker<GlobalConfig, PrimaryPatternConfig> Objects::primary_stats;
    Logger<GlobalConfig, LoggerConfig> Objects::logger;

}