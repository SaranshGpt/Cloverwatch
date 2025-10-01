//
// Created by saransh on 9/25/25.
//

#ifndef CLOVERWATCH_CLI_FUNCS_H
#define CLOVERWATCH_CLI_FUNCS_H

#include "../../CLI/include/cli.h"

namespace Cloverwatch::Pattern {

	Cli::CommandRes display_all_stats(Cli::Shell sh);

	Cli::CommandRes display_stats(Cli::Shell sh, Cli::Types::STR name);

	Cli::CommandRes define_pattern(Cli::Shell sh, Cli::Types::STR name, Cli::Types::BYTES notation);

	Cli::CommandRes enable_pattern(Cli::Shell sh, Cli::Types::STR name);

	Cli::CommandRes disable_pattern(Cli::Shell sh, Cli::Types::STR name);

	Cli::CommandRes remove_pattern(Cli::Shell sh, Cli::Types::STR name);

}

#endif //CLOVERWATCH_CLI_FUNCS_H