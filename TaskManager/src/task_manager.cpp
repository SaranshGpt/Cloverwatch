//
// Created by saransh on 6/16/25.
//

#include "task_manager.h"

namespace Cloverwatch {

    K_KERNEL_STACK_ARRAY_DEFINE(ThreadStacks, 5, 1024);

    TaskManager TaskManager::instance = TaskManager();

    static void add_task_delayed(TaskManager::DelayedContext *context, k_work_q* work_q, k_timeout_t delay);

    static void TaskHandler_Immidiate(k_work* work) {
        auto context = CONTAINER_OF(work, TaskManager::ImmediateContext, work);

        auto &task = context->task;
        task.func(task.args);

        delete context;
    }
    
    static void TaskHandler_Delayed(k_work* work) {
        auto delay_work = CONTAINER_OF(work, k_work_delayable, work);
        auto context = CONTAINER_OF(delay_work, TaskManager::DelayedContext, work);

        auto &task = context->task;
        task.func(task.args);

        context->num_repeats--;

        if (context->forever  || context->num_repeats > 0) {
            add_task_delayed(context, delay_work->queue, context->interval);
        }
        else {
            delete context;
        }
    }

    static void add_task_immidiate(TaskManager::ImmediateContext *context, k_work_q* work_q) {

        k_work_init(&context->work, TaskHandler_Immidiate);
        k_work_submit_to_queue(work_q, &context->work);
    }

    static void add_task_delayed(TaskManager::DelayedContext *context, k_work_q* work_q, k_timeout_t delay) {
        k_work_init_delayable(&context->work, TaskHandler_Delayed);
        k_work_schedule_for_queue(work_q, &context->work, delay);
    }

    void TaskManager::init() {

        int ind = 0;

        for (auto &[work_q, stack]: work_queues) {

            stack = ThreadStacks[ind];

            k_work_queue_init(&work_q);
            k_work_queue_start(&work_q, stack, 1024, priority_values[ind++], nullptr);
        }
    }

    void TaskManager::add_work_item(Task task) {
        auto context = new ImmediateContext {
            {},
            task
        };

        add_task_immidiate(context, &work_queues[static_cast<int>(task.priority)].work_q);
    }

    void TaskManager::add_work_item_delayed(Task task, k_timeout_t delay) {
        auto context = new DelayedContext {
            {},
            task,
            delay,
            1
        };

        add_task_delayed(context, &work_queues[static_cast<int>(task.priority)].work_q, delay);
    }

    void TaskManager::add_work_item_repeating(Task task, k_timeout_t period, uint16_t num_repeats) {
        auto context = new DelayedContext {
            {},
            task,
            period,
            num_repeats
        };

        add_task_delayed(context, &work_queues[static_cast<int>(task.priority)].work_q, period);
    }

    void TaskManager::forever_work_item(Task task, k_timeout_t period) {
        auto context = new DelayedContext {
            {},
            task,
            period,
            0,
            true
        };

        add_task_delayed(context, &work_queues[static_cast<int>(task.priority)].work_q, period);
    }

}