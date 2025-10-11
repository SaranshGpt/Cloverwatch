//
// Created by saransh on 9/25/25.
//

#include <zephyr/kernel.h>

#include "cli_funcs.h"
#include "pattern_stats.h"

#include "../../Config/system_config.h"

namespace Cloverwatch::Pattern {

    void collapse_whitespace(WriteStr str_ref) {

        auto& str = str_ref.ref;

        size_t ip_ind = 0;

        while (str[ip_ind] == ' ')
            ip_ind++;

        str[0] = str[ip_ind++];

        size_t i = 1;

        for (; i<str.len() && ip_ind < str.len(); i++) {
            if (str[i] == ' ' && str[i-1] == ' ') {
                while (str[ip_ind] == ' ') ip_ind++;
                str[i] = str[ip_ind++];
            }
        }

        str.set_len(i);
    }

    template <typename T>
    using CliVector = HeapVector<T, &Cli::heap>;

    Cli::CommandRes display_stats(Cli::Shell sh, Cli::Types::STR names) {

        collapse_whitespace(ToWrite<String>(names));

        size_t num_words = 0;

        for (size_t i=0; i < names.len(); i++) {
            num_words += names[i] == ' ';
        }

        Cli::Types::STR buffer;
        buffer.realloc(names.len());

        for (size_t i=0; i < names.len(); i++) {

            if (names[i] == ' ') {

                sh.print("Pattern %s\n", buffer.data());

                size_t num_instances = 0;
                FixedVector<Time, PrimaryPatternConfig::history_length> timestamps;

                auto res = Objects::primary_stats.get_pattern_info(ToRead(buffer.as_str()), ToWrite(num_instances), ToWrite(timestamps.as_vec()));

                switch (res) {
                    case StatResult::OK:
                        sh.print("\tNum instances: %d\n", num_instances);
                        sh.print("\tTimestamps: ");
                        for (auto& timestamp: timestamps)
                            sh.print(" %d", timestamp.milliseconds());
                        sh.print("\n\n");
                        break;
                    case StatResult::PATTERN_NOT_DEFINED:
                        sh.print<Cli::Shell::Level::ERROR>("Pattern with that name does not exist\n");
                        break;
                    default:
                        sh.print<Cli::Shell::Level::ERROR>("Unexpected error\n");
                        break;
                }

                buffer.clear();
                continue;
            }

            buffer.push_back(names[i]);

        }

        names.free();
        buffer.free();

        return Cli::CommandRes::SUCCESS;
    }

    Cli::CommandRes define_pattern(Cli::Shell sh, Cli::Types::STR name, Cli::Types::BYTES notation) {

        auto& primary_stats = Objects::primary_stats;

        auto res = primary_stats.add_pattern(ToCopy(name.as_str()), ToRead(notation.as_vec()));

        using Level = Cli::Shell::Level;

        switch (res) {
            case StatResult::OK:
                sh.print("Pattern %s defined\n", name.data());
                break;
            case StatResult::INSUFFICIENT_CAPACITY:
                sh.print<Level::ERROR>("Insufficient capacity for new definition\n");
                break;
            case StatResult::INVALID_NOTATION:
                sh.print<Level::ERROR>("Invalid notation\n");
                break;
            case StatResult::PATTERN_ALREADY_DEFINED:
                sh.print<Level::ERROR>("Pattern with that name already exists\n");
            default:
                sh.print<Level::ERROR>("Unexpected error\n");
                break;
        }

        name.free();
        notation.free();

        if (res == StatResult::OK) return Cli::CommandRes::SUCCESS;
        return Cli::CommandRes::EXECERROR;
    }

    Cli::CommandRes enable_pattern(Cli::Shell sh, Cli::Types::STR name) {
        auto& primary_stats = Objects::primary_stats;

        auto res = primary_stats.set_pattern_enabled(ToRead(name.as_str()), true);

        switch (res) {
            case StatResult::OK:
                sh.print("Pattern %s enabled\n", name.data());
                break;
            case StatResult::PATTERN_NOT_DEFINED:
                sh.print<Cli::Shell::Level::ERROR>("Pattern with that name does not exist\n");
                break;
            case StatResult::PATTERN_ALREADY_ENABLED:
                sh.print<Cli::Shell::Level::WARN>("Pattern is already enabled\n");
                break;
            default:
                sh.print<Cli::Shell::Level::ERROR>("Unexpected error\n");
                break;
        }

        name.free();

        if (res == StatResult::OK) return Cli::CommandRes::SUCCESS;
        return Cli::CommandRes::EXECERROR;
    }

    Cli::CommandRes disable_pattern(Cli::Shell sh, Cli::Types::STR name) {
        auto& primary_stats = Objects::primary_stats;

        auto res = primary_stats.set_pattern_enabled(ToRead(name.as_str()), false);

        switch (res) {
            case StatResult::OK:
                sh.print("Pattern %s disabled\n", name.data());
                break;
            case StatResult::PATTERN_NOT_DEFINED:
                sh.print<Cli::Shell::Level::ERROR>("Pattern with that name does not exist\n");
                break;
            case StatResult::PATTERN_NOT_ENABLED:
                sh.print<Cli::Shell::Level::WARN>("Pattern is already disabled\n");
                break;
            default:
                sh.print<Cli::Shell::Level::ERROR>("Unexpected error\n");
        }

        name.free();

        if (res == StatResult::OK) return Cli::CommandRes::SUCCESS;
        return Cli::CommandRes::EXECERROR;
    }

    Cli::CommandRes remove_pattern(Cli::Shell sh, Cli::Types::STR name) {

        sh.print<Cli::Shell::Level::ERROR>("Not implemented yet\n");

        name.free();

        return Cli::CommandRes::EXECERROR;
    }

}