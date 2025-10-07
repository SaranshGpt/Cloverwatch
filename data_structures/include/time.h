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

        Time() : _ticks(-1) {}

        [[nodiscard]] int64_t ticks() const {return _ticks;}

        [[nodiscard]] int64_t milliseconds() const {
            return _ticks / CONFIG_SYS_CLOCK_TICKS_PER_SEC * 1000;
        }

        [[nodiscard]] int64_t seconds() const {
            return _ticks / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
        }

        static Time UpTime() {
            return {k_uptime_ticks()};
        }

        static Time FromTicks(int64_t ticks) {
            return {ticks};
        }

        static Time FromSeconds(int64_t seconds) {
            return {seconds * CONFIG_SYS_CLOCK_TICKS_PER_SEC};
        }

        static Time FromMilliseconds(int64_t milliseconds) {
            return {milliseconds * CONFIG_SYS_CLOCK_TICKS_PER_SEC / 1000};
        }

    };

    inline void sleep(const Time duration) {
        k_sleep({duration.ticks()});
    }

    inline void usleep(const Time duration) {
        k_usleep(duration.ticks());
    }

}

#endif //TIME_H
