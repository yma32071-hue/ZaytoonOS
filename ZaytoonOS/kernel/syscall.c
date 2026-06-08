#include "kernel/syscall.h"
#include "kernel/kernel/printk.h"
#include "kernel/drivers/console.h"
#include "kernel/mm/mm.h"

static void sys_write(struct syscall_frame *frame)
{
    const char *buf = (const char *)frame->rsi;
    u64 count = frame->rdx;
    for (u64 i = 0; i < count && buf[i]; ++i) {
        console_write_char(buf[i]);
    }
    frame->rax = count;
}

static void sys_exit(struct syscall_frame *frame)
{
    (void)frame;
    printk("[syscall] process exited");
    while (1) {
        asm volatile("hlt");
    }
}

static void sys_brk(struct syscall_frame *frame)
{
    frame->rax = (u64)0;
}

static void sys_read(struct syscall_frame *frame)
{
    (void)frame;
    frame->rax = 0;
}

void syscall_dispatch(struct syscall_frame *frame)
{
    switch (frame->rax) {
    case SYS_WRITE:
        sys_write(frame);
        break;
    case SYS_EXIT:
        sys_exit(frame);
        break;
    case SYS_BRK:
        sys_brk(frame);
        break;
    case SYS_READ:
        sys_read(frame);
        break;
    default:
        printk("[syscall] unknown syscall %u", (unsigned int)frame->rax);
        frame->rax = (u64)-1;
        break;
    }
}
