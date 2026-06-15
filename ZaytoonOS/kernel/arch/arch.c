#include "kernel/arch/arch.h"
#include "kernel/arch/gdt.h"
#include "kernel/arch/idt.h"
#include "kernel/arch/tss.h"

void arch_setup(void)
{
    asm volatile("cli");
    gdt_init();
    tss_init();
    idt_init();
    asm volatile("sti");
}

void arch_halt(void)
{
    asm volatile("hlt");
}
