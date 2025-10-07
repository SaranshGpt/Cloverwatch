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

        size_t num_words = 1;

        for (size_t i=0; i<names.len(); i++)
            num_words += names[i] == ' ';

        auto requests = CliVector<StatRequest>(num_words);

        size_t prev_ind = 0;
        FixedStr<PrimaryPatternConfig::max_pattern_name_length> pattern_name;

        for (size_t i=0; i<names.len(); i++) {
            if (names[i] == ' ') {
                pattern_name.clear();
                for (size_t j=prev_ind; j<i; j++)
                    pattern_name.push_back(names[j]);

                prev_ind = i + 1;
                requests.push_back(StatRequest::create<&Cli::heap>(CopyStr(pattern_name.as_str()), PrimaryPatternConfig::max_pattern_name_length));
            }
        }

        auto& primary_stats = Objects::primary_stats;

        primary_stats.add_stat_request(ToWriteBuffer(requests));

        while (!primary_stats.clear_if_stat_request_fulfilled()) {
            sleep(Time::FromMilliseconds(100));
        }

        for (auto& request: requests) {

            sh.print("\nPattern: %s \n", request.name.data());

            if (request.result != StatResult::OK) {
                const char* error_str;

                switch (request.result) {
                    case StatResult::PATTERN_NOT_DEFINED :
                        error_str = "Pattern with that name does not exist";
                        break;
                    case StatResult::PATTERN_NOT_ENABLED :
                        error_str = "Pattern is not enabled";
                        break;
                    default:
                        error_str = "Unexpected error";
                }
                sh.print<Cli::Shell::Level::WARN>("Error: %s\n", error_str);
                continue;
            }
            else {
                sh.print("\tInstances since startup: %d\n", request.num_instances);

                sh.print("\tTimestamps: ");
                for (auto& timestamp: request.timestamps) {
                    sh.print("%d ", timestamp);
                }
                sh.print("\n");
            }

            request.free<&Cli::heap>();
        }

        names.free();

        return Cli::CommandRes::SUCCESS;
    }

    Cli::CommandRes define_pattern(Cli::Shell sh, Cli::Types::STR name, Cli::Types::BYTES notation) {

        auto& primary_stats = Objects::primary_stats;

        auto res = primary_stats.add_pattern(ToCopy(name), ToRead(notation));

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

        auto res = primary_stats.set_pattern_enabled(ToRead(name), true);

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

        auto res = primary_stats.set_pattern_enabled(ToRead(name), false);

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