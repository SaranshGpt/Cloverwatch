//
// Created by saransh on 9/25/25.
//

#ifndef CLOVERWATCH_CLI_CONFIG_H
#define CLOVERWATCH_CLI_CONFIG_H

#include <zephyr/shell/shell.h>

#include "../../CLI/include/cli.h"
#include "PatternMatcher/include/cli_funcs.h"
#include "Logger/include/cli_funcs.h"

namespace Cloverwatch::Cli {

    SHELL_STATIC_SUBCMD_SET_CREATE(pattern_set,
        SHELL_CMD(stats, NULL, "Prints stats for the named patterns. For mutiple patterns, input them space seperated in double quotes.", func_handler<Pattern::display_stats>),
        SHELL_CMD(define, NULL, "Defines a new pattern with given name and notation", func_handler<Pattern::define_pattern>),
        SHELL_CMD(remove, NULL, "Deletes pattern of given name", func_handler<Pattern::remove_pattern>),
        SHELL_SUBCMD_SET_END
    );

    SHELL_CMD_REGISTER(pattern, &pattern_set, "Pattern management commands", NULL);

    SHELL_STATIC_SUBCMD_SET_CREATE(logger_set,
        SHELL_CMD(print, NULL, "Prints stats for the logger", func_handler<display_logs>),
        SHELL_SUBCMD_SET_END
    );

    SHELL_CMD_REGISTER(logger, &logger_set, "Logger management commands", NULL);

}

#endif //CLOVERWATCH_CLI_CONFIG_H