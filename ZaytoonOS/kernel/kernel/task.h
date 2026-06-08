#ifndef ZAYTOONOS_KERNEL_TASK_H
#define ZAYTOONOS_KERNEL_TASK_H

#include "kernel/types.h"
#include "kernel/kernel/context.h"

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_WAITING,
    TASK_STOPPED,
} task_state_t;

typedef struct task {
    const char *name;
    task_state_t state;
    bool user_task;
    void (*entry)(void);
    void *stack;
    uint64_t user_entry;
    uint64_t user_stack;
    uint64_t *page_table;
    context_t context;
    struct task *next;
} task_t;

void task_init(void);
void task_add(void (*entry)(void), const char *name);
void task_add_user(uint64_t entry, uint64_t stack, uint64_t *page_table, const char *name);
void task_yield(void);
void task_run(void);

#endif /* ZAYTOONOS_KERNEL_TASK_H */
