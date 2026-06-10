#ifndef ZAYTOONOS_KERNEL_CONTEXT_H
#define ZAYTOONOS_KERNEL_CONTEXT_H

#include <stdint.h> // The official standard header for explicit variable sizes

typedef struct context {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t esp;
} context_t;

void context_switch(context_t *old_ctx, const context_t *new_ctx);

#endif /* ZAYTOONOS_KERNEL_CONTEXT_H */