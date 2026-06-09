#ifndef SCHED_H
#define SCHED_H

#include "kernel/types.h"

void sched_init(void);
void schedule(void);
u64 sched_ticks(void);

#endif /* SCHED_H */
