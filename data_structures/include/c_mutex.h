//
// Created by saransh on 7/28/25.
//

#ifndef C_MUTEX_H
#define C_MUTEX_H

#include <zephyr/kernel.h>

namespace Cloverwatch {

    class Mutex {

        k_mutex mutex = Z_MUTEX_INITIALIZER(mutex);

    public:

        Mutex(k_mutex mutex): mutex(mutex) {}

        void lock() {
            k_mutex_lock(&mutex, K_FOREVER);
        }
        void unlock() {
            k_mutex_unlock(&mutex);
        }

        Mutex() = default;

        template<typename T>
        void execute_function(T func) {
            lock();
            func();
            unlock();
        }

    };

    class MutexLock {

        Mutex& mtx;

    public:

        explicit MutexLock(Mutex& mtx): mtx(mtx) {
            this->mtx = mtx;
            mtx.lock();
        }
        ~MutexLock() {
            mtx.unlock();
        }

    };

}

#endif //C_MUTEX_H
