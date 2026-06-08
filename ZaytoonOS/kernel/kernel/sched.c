#include "kernel/kernel/sched.h"
#include "kernel/kernel/printk.h"
#include "kernel/kernel/task.h"

static unsigned int tick_count;

void sched_init(void)
{
    tick_count = 0;
    printk("[sched] scheduler initialized");
}

void schedule(void)
{
    tick_count += 1;
    if ((tick_count & 0xF) == 0) {
        printk("[sched] scheduler tick %u", tick_count);
    }
    task_yield();
}
