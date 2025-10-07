//
// Created by saransh on 9/18/25.
//

#include "../../Config/system_config.h"
#include "pattern.h"
#include "pattern_stats.h"

namespace Cloverwatch::Pattern {

    template class StatTracker<GlobalConfig, PrimaryPatternConfig>;

}