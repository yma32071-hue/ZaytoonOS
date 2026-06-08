#include <stddef.h>
#include <stdint.h>
#include "kernel/mm/mm.h"
#include "kernel/mm/pmm.h"
#include "kernel/mm/paging.h"
#include "kernel/kernel/printk.h"

extern char _heap_start;
extern char _heap_end;
static uintptr_t heap_current;
static uintptr_t heap_limit;

void mm_init(void)
{
    pmm_init();
    paging_init();

    heap_current = (uintptr_t)&_heap_start;
    heap_limit = (uintptr_t)&_heap_end;
    printk("[mm] heap initialized from %p .. %p", (void *)heap_current, (void *)heap_limit);
}

void *kmalloc(size_t size)
{
    return kmalloc_aligned(size, 8);
}

void *kmalloc_aligned(size_t size, size_t align)
{
    if (size == 0 || align == 0) {
        return NULL;
    }

    uintptr_t aligned = (heap_current + align - 1) & ~(uintptr_t)(align - 1);
    if (aligned + size > heap_limit) {
        printk("[mm] allocation failed: %u bytes", (unsigned int)size);
        return NULL;
    }

    void *result = (void *)aligned;
    heap_current = aligned + size;
    return result;
}
