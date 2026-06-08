#ifndef ZAYTOONOS_MAIN_H
#define ZAYTOONOS_MAIN_H

#include "kernel/types.h"

void arch_setup(void);
void irq_init(void);
void kernel_init(void);
void kernel_idle(void);

__attribute__((noreturn)) void kernel_start(void);

#endif /* ZAYTOONOS_MAIN_H */
