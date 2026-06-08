#include "maink.h"
#include "kernel/kernel/init.h"
#include "kernel/arch/arch.h"
#include "kernel/kernel/task.h"

void kernel_start(void)
{
    arch_setup();
    irq_init();
    kernel_init();
    task_run();

    while (1) {
        arch_halt();
    }
}
