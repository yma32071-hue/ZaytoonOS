#ifndef ZAYTOONOS_ARCH_GDT_H
#define ZAYTOONOS_ARCH_GDT_H

#include <stdint.h>

#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28

void gdt_init(void);
void gdt_set_tss(uint32_t base, uint32_t limit);

#endif /* ZAYTOONOS_ARCH_GDT_H */
