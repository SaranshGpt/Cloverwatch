//
// Created by saransh on 6/16/25.
//

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#include <array>
#include <vector>

namespace Cloverwatch
{

    namespace TaskManagerConfig {
        constexpr int STACK_SIZE = 2*1024;
    }

    class TaskManager {
    public:

        enum class Priority {
            IDLE    = 0,
            LOW     = 1,
            MEDIUM  = 2,
            HIGH    = 3,
            URGENT  = 4
        };

        using TaskFunc = void (*)(void*);

        struct Task {
            TaskFunc func;
            void* args;
            Priority priority = Priority::IDLE;
        };

        struct ImmediateContext {
            struct k_work work;
            Task task;
        };

        struct DelayedContext {
            struct k_work_delayable work;
            Task task;
            k_timeout_t interval;
            uint16_t num_repeats;
            bool forever=false;
        };

        void init();

        void add_work_item(Task task);

        void add_work_item_delayed(Task task, k_timeout_t delay);

        void add_work_item_repeating(Task task, k_timeout_t period, uint16_t num_repeats);

        void forever_work_item(Task task, k_timeout_t period);

        static TaskManager& Instance() { return instance; }

    private:

        TaskManager() {}

        static TaskManager instance;

        struct WorkQueue {
            k_work_q work_q;
            k_thread_stack_t* stack;
        };

        std::array<WorkQueue, 5> work_queues;

        std::array<int, 5> priority_values = {3, 2, 1, 0, -1};

    };
}

#endif //TASK_MANAGER_H
