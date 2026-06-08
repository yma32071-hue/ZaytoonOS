#ifndef ZAYTOONOS_KERNEL_CONTEXT_H
#define ZAYTOONOS_KERNEL_CONTEXT_H

#include "kernel/types.h"

typedef struct context {
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 rbx;
    u64 rbp;
    u64 rsp;
} context_t;

void context_switch(context_t *old, const context_t *new);

#endif /* ZAYTOONOS_KERNEL_CONTEXT_H */
