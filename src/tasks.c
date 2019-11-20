#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "tasks.h"

#define TASK_POOL_SIZE 12

struct task_handle_t {
    task_fn_t main_fn;
    task_argument_t arg;
    int32_t exec_period;
    rtc_time_t next_exec_time;
};

struct task_handle_t task_pool[TASK_POOL_SIZE];
static int next_free_task_idx = 0;
static int task_pool_usage = 0;


static bool is_free_task(struct task_handle_t *task)
{
    return task->main_fn == NULL;
}


static void update_next_free_task(void)
{
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        if (is_free_task(&task_pool[i])) {
            next_free_task_idx = i;
            return;
        }
    }

    next_free_task_idx = -1;
}


void task_init()
{
    /* must be called before the first tasks can be created */
    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task_destroy(&task_pool[i]);
    }
}

struct task_handle_t* task_create(task_fn_t main, task_argument_t *arg, int32_t exec_period)
{
    struct task_handle_t* retval;

    if (task_pool_usage == TASK_POOL_SIZE) {
        /* panic */
        return NULL;
    }

    if (next_free_task_idx == -1) {
        update_next_free_task();
    }

    task_pool_usage++;

    retval = task_pool + next_free_task_idx;
    retval->main_fn = main;
    if (arg) {
        retval->arg = *arg;
    } else {
        memset(&retval->arg, 0, sizeof(retval->arg));
    }
    retval->exec_period = exec_period;
    retval->next_exec_time = 0; /* to be calculated by scheduler */

    next_free_task_idx = -1;

    return retval;
}

int task_schedule(rtc_time_t *next_runtime)
{
    int exec_count = 0;
    struct task_handle_t* task;
    rtc_time_t now = rtc_current_time;

    for (int i = 0; i < TASK_POOL_SIZE; i++) {
        task = &task_pool[i];

        if (is_free_task(task)) {
            continue;
        }

        if (task->next_exec_time == 0) {
            if (task->exec_period == 0) {
                /* single shot task */
                task->next_exec_time = now;
            } else if (task->exec_period < 0) {
                /* task with fixed (absolute) execution time */
                task->next_exec_time = -task->exec_period;
            } else if (task->exec_period > 0) {
                /* periodical task */
                task->next_exec_time = now + task->exec_period;
            }
        }

        if (task->next_exec_time <= now) {
            task->main_fn(task->arg);
            exec_count++;

            if (task->exec_period <= 0) {
                /* single shot task, free task struct */
                task_destroy(task);
            } else if (task->exec_period > 0) {
                /* re-schedule next execution of periodic task */
                task->next_exec_time += task->exec_period;
            }
        } else {
            if (next_runtime && task->next_exec_time < *next_runtime) {
                *next_runtime = task->next_exec_time;
            }
        }
    }

    if (exec_count > 0) {
        /* start atomic */
        update_next_free_task();
        /* end_atomic */
    }

    return exec_count;
}


void task_destroy(struct task_handle_t* task)
{
    task->main_fn = NULL;
    task->exec_period = 0;
    task->next_exec_time = 0;

    task_pool_usage--;

    return;
}
