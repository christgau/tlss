#ifndef TASKS_H
#define TASKS_H

#include "rtc.h"

#define TASK_IN(x) (rtc_current_time + (x))
#define TASK_ABSOLUTE(x) ((x) > 0 ? -(x) : (x))
#define TASK_PERIODICAL(x) ((x) < 0 ? -(x) : (x))
#define TASK_NOW (0)

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    uint8_t byte_arg;
    uint16_t short_arg;
    uint32_t int_arg;
    void* ptr_arg;
} task_argument_t;

typedef void (*task_fn_t) (task_argument_t arg);

struct task_handle_t;

/* init task framework */
void task_init();

/* create a new task */
struct task_handle_t* task_create(task_fn_t main, task_argument_t *arg, int32_t exec_period);

/* schedule and execute tasks that are due */
int task_schedule(rtc_time_t *next_runtime);

/* destroy  */
void task_destroy(struct task_handle_t* task);

#ifdef __cplusplus
}
#endif

#endif
