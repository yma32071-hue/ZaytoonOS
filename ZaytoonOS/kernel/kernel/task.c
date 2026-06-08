#include "kernel/arch/arch.h"
#include "kernel/arch/user_entry.h"
#include "kernel/kernel/context.h"
#include "kernel/kernel/task.h"
#include "kernel/kernel/printk.h"
#include "kernel/mm/mm.h"

#define KERNEL_TASK_STACK_SIZE 8192

static task_t *current_task;
static task_t *task_list;
static context_t scheduler_context;

static void task_wrapper(void)
{
    if (current_task && current_task->entry) {
        current_task->entry();
    }

    current_task->state = TASK_STOPPED;
    task_yield();

    for (;;) {
        arch_halt();
    }
}

void task_init(void)
{
    task_list = NULL;
    current_task = NULL;
}

static inline void load_cr3(uint64_t *cr3)
{
    uint64_t value = (uint64_t)cr3;
    asm volatile("mov %0, %%cr3" :: "r"(value));
}

void task_add(void (*entry)(void), const char *name)
{
    task_t *task = kmalloc(sizeof(task_t));
    void *stack = kmalloc(KERNEL_TASK_STACK_SIZE);
    if (!task || !stack) {
        printk("[task] allocation failed for %s", name);
        return;
    }

    u64 stack_top = (u64)stack + KERNEL_TASK_STACK_SIZE;
    stack_top &= ~0xFul;
    stack_top -= sizeof(u64);
    *(u64 *)stack_top = (u64)task_wrapper;

    task->name = name;
    task->user_task = false;
    task->state = TASK_READY;
    task->entry = entry;
    task->stack = stack;
    task->user_entry = 0;
    task->user_stack = 0;
    task->page_table = NULL;
    task->next = NULL;
    task->context.r15 = 0;
    task->context.r14 = 0;
    task->context.r13 = 0;
    task->context.r12 = 0;
    task->context.rbx = 0;
    task->context.rbp = 0;
    task->context.rsp = stack_top;

    if (!task_list) {
        task_list = task;
    } else {
        task_t *tail = task_list;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = task;
    }

    printk("[task] registered %s", name);
}

void task_add_user(uint64_t entry, uint64_t stack, uint64_t *page_table, const char *name)
{
    task_t *task = kmalloc(sizeof(task_t));
    if (!task) {
        printk("[task] allocation failed for %s", name);
        return;
    }

    task->name = name;
    task->user_task = true;
    task->state = TASK_READY;
    task->entry = NULL;
    task->stack = NULL;
    task->user_entry = entry;
    task->user_stack = stack;
    task->page_table = page_table;
    task->next = NULL;
    task->context.r15 = 0;
    task->context.r14 = 0;
    task->context.r13 = 0;
    task->context.r12 = 0;
    task->context.rbx = 0;
    task->context.rbp = 0;
    task->context.rsp = 0;

    if (!task_list) {
        task_list = task;
    } else {
        task_t *tail = task_list;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = task;
    }

    printk("[task] registered user task %s", name);
}

void task_yield(void)
{
    if (!current_task) {
        return;
    }

    task_t *prev = current_task;
    task_t *next = prev->next ? prev->next : task_list;
    if (next == prev) {
        return;
    }

    prev->state = TASK_READY;
    current_task = next;
    current_task->state = TASK_RUNNING;
    printk("[task] switching to %s", current_task->name);
    context_switch(&prev->context, &current_task->context);
}

void task_run(void)
{
    if (!task_list) {
        return;
    }

    current_task = task_list;
    current_task->state = TASK_RUNNING;
    printk("[task] starting %s", current_task->name);
    if (current_task->user_task) {
        load_cr3(current_task->page_table);
        printk("[task] entering user task %s", current_task->name);
        enter_user_mode(current_task->user_entry, current_task->user_stack);
    } else {
        context_switch(&scheduler_context, &current_task->context);
    }
}
