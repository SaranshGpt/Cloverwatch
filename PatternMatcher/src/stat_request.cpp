//
// Created by saransh on 10/1/25.
//

#include <pattern_stats.h>

#include "pattern_stats.h"

namespace Cloverwatch::Pattern {


    template<typename G, typename L>
    StatResult StatTracker<G, L>::get_pattern_info(ReadStr name, WriteRef<size_t> num_instances, WriteVector<Time> timestamps) {

        for (auto& pattern: patterns) {

            pattern.mtx.lock();
            if (!pattern.defined) continue;
            if (name.ref != pattern.name.as_str()) continue;

            num_instances.ref = pattern.num_instances;
            auto timestamps_cpy = pattern.timestamps.get_vec();

            pattern.mtx.unlock();

            for (auto& timestamp: timestamps_cpy) {
                timestamps.ref.push_back(timestamp);
            }

            return StatResult::OK;
        }

        return StatResult::PATTERN_NOT_DEFINED;

    }


    template<typename G, typename L>
    StatResult StatTracker<G, L>::set_pattern_enabled(ReadStr name, bool enabled) {


        for (auto& pattern: patterns) {

            auto lock = MutexLock(pattern.mtx);

            if (!pattern.defined) continue;

            if (name.ref != pattern.name.as_str()) continue;

            if (pattern.enabled == enabled) {
                if (enabled) return StatResult::PATTERN_ALREADY_ENABLED;
                else return StatResult::PATTERN_NOT_ENABLED;
            }

            pattern.enabled = enabled;
            return StatResult::OK;
        }

        return StatResult::PATTERN_NOT_DEFINED;
    }

}

#include "../compile_time_init/pattern_template_config.cpp"