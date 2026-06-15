#include <stdint.h>
#include "kernel/mm/paging.h"
#include "kernel/mm/pmm.h"
#include "kernel/kernel/printk.h"

uint64_t *paging_create_user_space(void)
{
    return NULL;
}

void paging_map_region(uint64_t *pml4, uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags)
{
    (void)pml4; (void)virt; (void)phys; (void)size; (void)flags;
}

void paging_init(void)
{
    printk("[paging] flat identity map (32-bit mode)");
}
