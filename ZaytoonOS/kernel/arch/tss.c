#include "kernel/arch/tss.h"
#include "kernel/arch/gdt.h"
#include "kernel/types.h"

struct tss32_entry {
    u32 prev_tss;
    u32 esp0;
    u32 ss0;
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax, ecx, edx, ebx;
    u32 esp, ebp, esi, edi;
    u32 es, cs, ss, ds, fs, gs;
    u32 ldt;
    u16 trap;
    u16 io_map_base;
} __attribute__((packed));

static struct tss32_entry tss;
static u8 kernel_stack[8192] __attribute__((aligned(16)));

void tss_init(void)
{
    for (unsigned int i = 0; i < sizeof(tss); ++i) {
        ((u8 *)&tss)[i] = 0;
    }

    tss.ss0  = GDT_KERNEL_DATA;
    tss.esp0 = (u32)((u8 *)kernel_stack + sizeof(kernel_stack));
    tss.io_map_base = sizeof(tss);

    gdt_set_tss((u32)&tss, sizeof(tss) - 1);
    asm volatile("ltr %%ax" :: "a"((u16)GDT_TSS));
}
