//
// Created by saransh on 10/1/25.
//

#include "pattern_stats.h"

namespace Cloverwatch::Pattern {

    template <typename G, typename L>
    StatResult StatTracker<G, L>::add_stat_request(WriteBufferVector<StatRequest> stat_requests) {

        auto lock = MutexLock(request_mtx);

        if (current_request) return StatResult::REQUEST_ALREADY_EXISTS;

        current_request = &stat_requests.ref;
        return StatResult::OK;
    }

    template <typename G, typename L>
    bool StatTracker<G, L>::clear_if_stat_request_fulfilled() {
        auto lock = MutexLock(request_mtx);

        for (auto& request: *current_request) {
            if (request.result == StatResult::REQUEST_NOT_FULFILLED) {
                return false;
            }
        }

        current_request = nullptr;

        return true;
    }

    template<typename G, typename L>
    StatResult StatTracker<G, L>::set_pattern_enabled(ReadStr name, bool enabled) {

        auto lock = MutexLock(pattern_mtx);

        for (auto& pattern: patterns) {

            if (!pattern.defined) continue;

            if (name != pattern.name) continue;

            if (pattern.enabled == enabled) {
                if (enabled) return StatResult::PATTERN_ALREADY_ENABLED;
                else return StatResult::PATTERN_NOT_ENABLED;
            }

            pattern.enabled = enabled;
            return StatResult::OK;
        }

        return StatResult::PATTERN_NOT_FOUND;
    }

}