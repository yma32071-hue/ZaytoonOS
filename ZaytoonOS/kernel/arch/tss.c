#include "kernel/arch/tss.h"
#include "kernel/arch/gdt.h"
#include "kernel/kernel/printk.h"
#include "kernel/types.h"

struct tss_entry {
    u32 reserved0;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved1;
    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;
    u64 reserved2;
    u16 reserved3;
    u16 io_map_base;
} __attribute__((packed));

static struct tss_entry tss;
static u8 kernel_stack[8192] __attribute__((aligned(16)));

void tss_init(void)
{
    for (unsigned int i = 0; i < sizeof(tss); ++i) {
        ((u8 *)&tss)[i] = 0;
    }

    tss.rsp0 = (u64)kernel_stack + sizeof(kernel_stack);
    tss.io_map_base = sizeof(tss);

    gdt_set_tss((uint64_t)&tss, sizeof(tss) - 1);
    asm volatile("ltr %%ax" :: "a"((uint16_t)0x28));
    printk("[tss] task state segment loaded");
}
