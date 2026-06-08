#include "kernel/arch/arch.h"
#include "kernel/arch/gdt.h"
#include "kernel/arch/idt.h"
#include "kernel/arch/tss.h"
#include "kernel/arch/msr.h"
#include "kernel/kernel/printk.h"

extern void syscall_entry(void);

static void setup_syscall(void)
{
    const uint64_t efer = read_msr(0xC0000080);
    write_msr(0xC0000080, efer | 0x1);

    uint64_t star = ((uint64_t)GDT_USER_CODE << 48) | ((uint64_t)GDT_KERNEL_CODE << 32);
    write_msr(0xC0000081, star);
    write_msr(0xC0000082, (uint64_t)syscall_entry);
    write_msr(0xC0000084, 0x200);
}

void arch_setup(void)
{
    asm volatile("cli");
    gdt_init();
    tss_init();
    idt_init();
    setup_syscall();
    printk("[arch] CPU descriptor tables configured");
    asm volatile("sti");
}

void arch_halt(void)
{
    asm volatile("hlt");
}
