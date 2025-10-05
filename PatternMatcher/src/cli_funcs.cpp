//
// Created by saransh on 9/25/25.
//

#include "cli_funcs.h"
#include "pattern_stats.h"

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
        FixedStr<PatternConfig::max_pattern_name_length> pattern_name;

        for (size_t i=0; i<names.len(); i++) {
            if (names[i] == ' ') {
                pattern_name.clear();
                for (size_t j=prev_ind; j<i; j++)
                    pattern_name.push_back(names[j]);

                prev_ind = i + 1;
                requests.push_back(StatRequest::create<&Cli::heap>(CopyStr(pattern_name.as_str()), PatternConfig::max_pattern_name_length));
            }
        }

        Stats::add_stat_request(requests);

    }

}