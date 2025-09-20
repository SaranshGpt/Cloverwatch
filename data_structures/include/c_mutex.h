//
// Created by saransh on 7/28/25.
//

#ifndef C_MUTEX_H
#define C_MUTEX_H

#include <zephyr/kernel.h>

namespace Cloverwatch {

    class Mutex {

        // ReSharper disable once CppUninitializedNonStaticDataMember
        k_mutex mutex;

    public:

        void init();

        void lock();
        void unlock();

        Mutex();
    };

    class MutexLock {

        Mutex& mtx;

    public:

        MutexLock(Mutex& mtx): mtx(mtx) {
            this->mtx = mtx;
            mtx.lock();
        }
        ~MutexLock() {
            mtx.unlock();
        }

    };

}

#endif //C_MUTEX_H
