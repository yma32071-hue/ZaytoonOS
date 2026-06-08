#include <stdint.h>
#include "kernel/mm/paging.h"
#include "kernel/mm/pmm.h"
#include "kernel/kernel/printk.h"

#define PAGE_PRESENT 0x001
#define PAGE_WRITE   0x002
#define PAGE_USER    0x004
#define PAGE_PWT     0x008
#define PAGE_PCD     0x010
#define PAGE_ACCESSED 0x020
#define PAGE_DIRTY   0x040
#define PAGE_PS      0x080

static uint64_t *kernel_pml4;
static uint64_t *kernel_pdpt;
static uint64_t *kernel_pd;

static uint64_t *alloc_page_table(void)
{
    void *frame = pmm_alloc_frame();
    if (!frame) {
        return NULL;
    }
    uint64_t *page = (uint64_t *)frame;
    for (unsigned int i = 0; i < 512; ++i) {
        page[i] = 0;
    }
    return page;
}

static void map_2mb_region(uint64_t *pd, uint64_t virt, uint64_t phys, uint64_t flags)
{
    uint64_t index = (virt >> 21) & 0x1FF;
    pd[index] = (phys & 0xFFFFFFFFFFE00000ULL) | flags | PAGE_PS;
}

void paging_map_region(uint64_t *pml4, uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags)
{
    for (uint64_t offset = 0; offset < size; offset += 0x200000) {
        uint64_t vaddr = virt + offset;
        uint64_t pml4_index = (vaddr >> 39) & 0x1FF;
        uint64_t pdpt_index = (vaddr >> 30) & 0x1FF;
        uint64_t pd_index = (vaddr >> 21) & 0x1FF;

        if (!(pml4[pml4_index] & PAGE_PRESENT)) {
            uint64_t *new_pdpt = alloc_page_table();
            pml4[pml4_index] = ((uint64_t)new_pdpt) | PAGE_PRESENT | PAGE_WRITE;
        }

        uint64_t *pdpt = (uint64_t *)(pml4[pml4_index] & ~0xFFFULL);
        if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
            uint64_t *new_pd = alloc_page_table();
            pdpt[pdpt_index] = ((uint64_t)new_pd) | PAGE_PRESENT | PAGE_WRITE;
        }

        uint64_t *pd = (uint64_t *)(pdpt[pdpt_index] & ~0xFFFULL);
        pd[pd_index] = (phys & 0xFFFFFFFFFFE00000ULL) | flags | PAGE_PS;
    }
}

uint64_t *paging_create_user_space(void)
{
    uint64_t *pml4 = alloc_page_table();
    if (!pml4) {
        return NULL;
    }
    for (unsigned int i = 0; i < 512; ++i) {
        pml4[i] = 0;
    }

    pml4[0] = kernel_pml4[0];

    return pml4;
}

void paging_init(void)
{
    kernel_pml4 = alloc_page_table();
    kernel_pdpt = alloc_page_table();
    kernel_pd = alloc_page_table();
    if (!kernel_pml4 || !kernel_pdpt || !kernel_pd) {
        printk("[paging] failed to allocate page tables");
        return;
    }

    kernel_pml4[0] = ((uint64_t)kernel_pdpt) | PAGE_PRESENT | PAGE_WRITE;
    kernel_pdpt[0] = ((uint64_t)kernel_pd) | PAGE_PRESENT | PAGE_WRITE;

    for (uint64_t address = 0; address < 0x4000000ULL; address += 0x200000ULL) {
        map_2mb_region(kernel_pd, address, address, PAGE_PRESENT | PAGE_WRITE);
    }

    paging_map_region(kernel_pml4, 0x400000ULL, 0x400000ULL, 0x200000ULL, PAGE_PRESENT | PAGE_WRITE);
    paging_map_region(kernel_pml4, 0x70000000ULL, 0x70000000ULL, 0x200000ULL, PAGE_PRESENT | PAGE_WRITE);

    uint64_t cr3 = (uint64_t)kernel_pml4;
    asm volatile("mov %0, %%cr3" :: "r"(cr3));
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1ULL << 5);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1ULL << 31) | (1ULL << 0);
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
    printk("[paging] identity paging enabled");
}
