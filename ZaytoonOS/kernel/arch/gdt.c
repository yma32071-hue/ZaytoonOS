#include "kernel/arch/gdt.h"
#include "kernel/kernel/printk.h"
#include "kernel/types.h"
#include <string.h>

struct gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed));

struct gdt_entry_long {
    u32 base_upper;
    u32 reserved;
} __attribute__((packed));

struct gdt_ptr {
    u16 limit;
    u64 base;
} __attribute__((packed));

static struct gdt_entry gdt[5];
static struct gdt_entry_long gdt_tss_long;
static unsigned char gdt_table[sizeof(gdt) + sizeof(gdt_tss_long)];
static struct gdt_ptr gdt_descriptor;

static void set_gdt_entry(int index, u32 base, u32 limit, u8 access, u8 gran)
{
    gdt[index].limit_low    = (u16)(limit & 0xFFFF);
    gdt[index].base_low     = (u16)(base & 0xFFFF);
    gdt[index].base_middle  = (u8)((base >> 16) & 0xFF);
    gdt[index].access       = access;
    gdt[index].granularity  = (u8)((limit >> 16) & 0x0F);
    gdt[index].granularity |= gran & 0xF0;
    gdt[index].base_high    = (u8)((base >> 24) & 0xFF);
}

void gdt_set_tss(uint64_t base, uint32_t limit)
{
    set_gdt_entry(4, (u32)(base & 0xFFFFFFFFu), limit, 0x89, 0x00);
    gdt_tss_long.base_upper = (uint32_t)(base >> 32);
    gdt_tss_long.reserved = 0;
}

void gdt_init(void)
{
    memset(&gdt, 0, sizeof(gdt));
    set_gdt_entry(0, 0, 0, 0, 0);
    set_gdt_entry(1, 0, 0x000FFFFF, 0x9A, 0xA0);
    set_gdt_entry(2, 0, 0x000FFFFF, 0x92, 0xA0);
    set_gdt_entry(3, 0, 0x000FFFFF, 0xFA, 0xA0);
    set_gdt_entry(4, 0, 0, 0, 0);

    memcpy(gdt_table, gdt, sizeof(gdt));
    memcpy(gdt_table + sizeof(gdt), &gdt_tss_long, sizeof(gdt_tss_long));

    gdt_descriptor.limit = sizeof(gdt_table) - 1;
    gdt_descriptor.base = (u64)gdt_table;

    asm volatile("lgdt %0" : : "m"(gdt_descriptor));
    printk("[arch] GDT loaded");
}
