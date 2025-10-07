//
// Created by saransh on 9/25/25.
//

#ifndef CLOVERWATCH_CLI_FUNCS_H
#define CLOVERWATCH_CLI_FUNCS_H

#include <pattern_stats.h>

#include "../../CLI/include/cli.h"

namespace Cloverwatch::Pattern {

	using Stats = StatTracker<GlobalConfig, PrimaryPatternConfig>;

	Cli::CommandRes display_stats(Cli::Shell sh, Cli::Types::STR names);

	Cli::CommandRes define_pattern(Cli::Shell sh, Cli::Types::STR name, Cli::Types::BYTES notation);

	Cli::CommandRes enable_pattern(Cli::Shell sh, Cli::Types::STR name);

	Cli::CommandRes disable_pattern(Cli::Shell sh, Cli::Types::STR name);

	Cli::CommandRes remove_pattern(Cli::Shell sh, Cli::Types::STR name);

}

#endif //CLOVERWATCH_CLI_FUNCS_H