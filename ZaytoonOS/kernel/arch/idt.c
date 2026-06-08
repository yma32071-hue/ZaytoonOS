#include "kernel/arch/idt.h"
#include "kernel/irq/irq.h"
#include "kernel/irq/pic.h"
#include "kernel/kernel/printk.h"

struct idt_entry {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 zero;
} __attribute__((packed));

struct idt_ptr {
    u16 limit;
    u64 base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idt_descriptor;

extern void default_interrupt_stub(void);
extern void timer_interrupt_stub(void);

static void set_idt_gate(u8 vector, void *handler, u16 selector, u8 type_attr)
{
    u64 handler_addr = (u64)handler;
    idt[vector].offset_low  = (u16)(handler_addr & 0xFFFF);
    idt[vector].selector    = selector;
    idt[vector].ist         = 0;
    idt[vector].type_attr   = type_attr;
    idt[vector].offset_mid  = (u16)((handler_addr >> 16) & 0xFFFF);
    idt[vector].offset_high = (u32)((handler_addr >> 32) & 0xFFFFFFFF);
    idt[vector].zero        = 0;
}

void default_interrupt_handler(void)
{
}

void timer_interrupt_handler(void)
{
    irq_dispatch(32);
    pic_eoi(0);
}

void idt_init(void)
{
    for (u16 vector = 0; vector < 256; ++vector) {
        set_idt_gate((u8)vector, default_interrupt_stub, 0x08, 0x8E);
    }

    set_idt_gate(32, timer_interrupt_stub, 0x08, 0x8E);

    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (u64)&idt;
    asm volatile("lidt %0" : : "m"(idt_descriptor));
    printk("[arch] IDT initialized");
}
