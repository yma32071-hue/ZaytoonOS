#include "kernel/arch/gdt.h"
#include "kernel/types.h"

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8  base_mid;
    u8  access;
    u8  gran;
    u8  base_high;
} __attribute__((packed));

struct gdt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

#define GDT_ENTRIES 6
unsigned char gdt_table[GDT_ENTRIES * 8];
static struct gdt_ptr gdt_descriptor;

static void set_entry(int index, u32 base, u32 limit, u8 access, u8 gran)
{
    struct gdt_entry *e = (struct gdt_entry *)&gdt_table[index * 8];
    e->limit_low = (u16)(limit & 0xFFFF);
    e->base_low  = (u16)(base & 0xFFFF);
    e->base_mid  = (u8)((base >> 16) & 0xFF);
    e->access    = access;
    e->gran      = (u8)(((limit >> 16) & 0x0F) | (gran & 0xF0));
    e->base_high = (u8)((base >> 24) & 0xFF);
}

void gdt_set_tss(u32 base, u32 limit)
{
    set_entry(5, base, limit, 0x89, 0x00);
}

void gdt_init(void)
{
    set_entry(0, 0, 0, 0, 0);
    set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);
    set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);
    set_entry(3, 0, 0xFFFFF, 0xFA, 0xCF);
    set_entry(4, 0, 0xFFFFF, 0xF2, 0xCF);
    set_entry(5, 0, 0, 0, 0);

    gdt_descriptor.limit = sizeof(gdt_table) - 1;
    gdt_descriptor.base  = (u32)gdt_table;

    asm volatile(
        "lgdt %0\n\t"
        "ljmp $0x08, $1f\n\t"
        "1:\n\t"
        "mov $0x10, %%eax\n\t"
        "mov %%eax, %%ds\n\t"
        "mov %%eax, %%es\n\t"
        "mov %%eax, %%fs\n\t"
        "mov %%eax, %%gs\n\t"
        "mov %%eax, %%ss\n\t"
        : : "m"(gdt_descriptor) : "eax", "memory"
    );
}
