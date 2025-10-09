//
// Created by saransh on 9/22/25.
//

#ifndef CLOVERWATCH_CLI_H
#define CLOVERWATCH_CLI_H

#include <zephyr/shell/shell.h>

#include <tuple>

#include "../../data_structures/include/c_string.h"
#include "../../data_structures/include/c_heap_string.h"

namespace Cloverwatch::Cli {

    DECLARE_HEAP(heap);

    enum class CommandRes {
        SUCCESS = 0,
        HELP = 1,
        ARGERROR = EINVAL,
        EXECERROR = ENOEXEC
    };

    namespace Types {
        using INT = int64_t;
        using FLOAT = double;
        using STR = HeapString<&heap>;
        using BYTES = HeapVector<Byte, &heap>;
        using BOOL = bool;
    }

    class Shell {
        const shell* sh;
    public:

        enum class Level {
            NORMAL = SHELL_NORMAL,
            INFO = SHELL_INFO,
            WARN = SHELL_WARNING,
            ERROR = SHELL_ERROR
        };

        explicit Shell(const shell* sh): sh(sh) {}

        template <Level level = Level::NORMAL, typename... Args>
        [[gnu::always_inline]] inline void print(const char* fmt, Args... args) {
            shell_fprintf(sh, static_cast<enum shell_vt100_color>(level), fmt, args...);
        }
    };

    template <typename T>
    bool parse_args(ReadRef<char*> arg, T& val, bool &res);

    template <typename T>
    struct CommandArgs;

    template <typename ...Args>
    struct CommandArgs<CommandRes(*)(Shell, Args...)> {
        using ArgsTuple = std::tuple<Args...>;
        using ReturnType = CommandRes;
        static constexpr size_t arg_count = sizeof...(Args);

        static bool fill_args(ArgsTuple& args, size_t argc, char** argv) {

            auto curr_arg = argv;
            bool res = true;

            (parse_args(ReadRef<char*>(*(curr_arg++)), std::get<Args>(args), res) && ...);

            return res;
        }
    };

    template <auto Func>
    int func_handler(const shell* sh, size_t argc, char** argv) {

        using CommandArgs = CommandArgs<decltype(Func)>;
        using ArgsTuple = typename CommandArgs::ArgsTuple;

        if (argc < CommandArgs::arg_count) return static_cast<int>(CommandRes::ARGERROR);

        auto args = ArgsTuple();

        if (!CommandArgs::fill_args(args, argc, argv)) return static_cast<int>(CommandRes::ARGERROR);

        auto res = std::apply([&sh](auto&&... tp_args) {
            return Func(Shell(sh), std::forward<decltype(tp_args)>(tp_args)...);
        }, args);

        return static_cast<int>(res);
    }

}

#endif //CLOVERWATCH_CLI_H