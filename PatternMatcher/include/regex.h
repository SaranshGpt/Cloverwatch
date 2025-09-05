//
// Created by saransh on 9/5/25.
//

#ifndef REGEX_H
#define REGEX_H

#include "../../data_structures/include/c_types.h"
#include "../compile_time_init/regex_config.h"

namespace Cloverwatch::Regex {

    using Pattern = ReadVector<Byte>;
    using Packet = ReadVector<Byte>;

    bool match(Pattern pattern, Packet packet);
    bool is_valid_pattern(Pattern pattern);

}

#endif //REGEX_H
