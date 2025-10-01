//
// Created by saransh on 9/25/25.
//

#ifndef CLOVERWATCH_CLI_CONFIG_H
#define CLOVERWATCH_CLI_CONFIG_H

#include <zephyr/shell/shell.h>

#include "../../CLI/include/cli.h"
#include "PatternMatcher/include/cli_funcs.h"

namespace Cloverwatch::Cli {

    SHELL_STATIC_SUBCMD_SET_CREATE(pattern,
        SHELL_CMD(all_stats, NULL, "Prints stats for all defined patterns", func_handler<Pattern::display_all_stats>),
        SHELL_CMD(stats, NULL, "Prints stats for the named pattern", func_handler<Pattern::display_stats>),
        SHELL_CMD(define, NULL, "Defines a new pattern with given name and notation", func_handler<Pattern::define_pattern>),
        SHELL_CMD(remove, NULL, "Deletes pattern of given name", func_handler<Pattern::remove_pattern>),
        SHELL_SUBCMD_SET_END
    );



}

#endif //CLOVERWATCH_CLI_CONFIG_H