//
// Created by saransh on 9/15/25.
//

#ifndef TIME_H
#define TIME_H

#include <zephyr/kernel.h>

namespace Cloverwatch {

    class Time {

        int64_t _ticks;

        Time(int64_t ticks): _ticks(ticks) {}

    public:

        int64_t ticks() const {return _ticks;}

        int64_t milliseconds() const {
            return _ticks / CONFIG_SYS_CLOCK_TICKS_PER_SEC * 1000;
        }

        int64_t seconds() const {
            return _ticks / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
        }

        static Time UpTime() {
            return Time(k_uptime_ticks());
        }

        static Time FromTicks(int64_t ticks) {
            return Time(ticks);
        }

        static Time FromSeconds(int64_t seconds) {
            return Time(seconds * CONFIG_SYS_CLOCK_TICKS_PER_SEC);
        }

        static Time FromMilliseconds(int64_t milliseconds) {
            return Time(milliseconds * CONFIG_SYS_CLOCK_TICKS_PER_SEC / 1000);
        }

    };

}

#endif //TIME_H
