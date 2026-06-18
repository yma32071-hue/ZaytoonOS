#include "kernel/kernel/sched.h"
#include "kernel/kernel/task.h"

static unsigned int tick_count;

void sched_init(void)
{
    tick_count = 0;
}

void schedule(void)
{
    tick_count += 1;
    task_yield();
}

u64 sched_ticks(void)
{
    return tick_count;
}
