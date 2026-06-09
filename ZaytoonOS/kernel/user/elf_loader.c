#include "kernel/user/elf_loader.h"
#include "kernel/mm/paging.h"
#include "kernel/mm/pmm.h"
#include "kernel/kernel/printk.h"
#include "kernel/types.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ELF_MAGIC 0x464C457F
#define PT_LOAD 1
#define PAGE_PRESENT 0x001
#define PAGE_WRITE   0x002
#define PAGE_USER    0x004

struct elf_header {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} __attribute__((packed));

struct elf_phdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} __attribute__((packed));

static bool valid_elf(const struct elf_header *header)
{
    uint32_t magic = *(const uint32_t *)header->e_ident;
    return magic == ELF_MAGIC && header->e_machine == 0x3E;
}

int elf_load_image(const unsigned char *image, size_t size, struct elf_process *process)
{
    if (!image || !process || size < sizeof(struct elf_header)) {
        return -1;
    }

    const struct elf_header *header = (const struct elf_header *)image;
    if (!valid_elf(header) || header->e_phoff + ((uint64_t)header->e_phnum * header->e_phentsize) > size) {
        return -1;
    }

    process->entry = header->e_entry;
    process->page_table = paging_create_user_space();
    if (!process->page_table) {
        printk("[elf] failed to create page table");
        return -1;
    }

    for (uint16_t i = 0; i < header->e_phnum; ++i) {
        const struct elf_phdr *ph = (const struct elf_phdr *)(image + header->e_phoff + i * header->e_phentsize);
        if (ph->p_offset + ph->p_filesz > size) {
            printk("[elf] segment outside image");
            return -1;
        }
        if (ph->p_type != PT_LOAD) {
            continue;
        }

        uint64_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        if (ph->p_flags & 0x1) {
            flags |= 0x4;
        }

        uint64_t region_start = ph->p_vaddr & ~0x1FFFFFULL;
        uint64_t region_size = ((ph->p_vaddr + ph->p_memsz + 0x1FFFFFULL) & ~0x1FFFFFULL) - region_start;
        uint64_t page_count = region_size >> 21;

        for (uint64_t page = 0; page < page_count; ++page) {
            uint64_t addr = region_start + (page << 21);
            void *frame = pmm_alloc_frame();
            if (!frame) {
                printk("[elf] out of frames");
                return -1;
            }
            paging_map_region(process->page_table, addr, (uint64_t)frame, 0x200000ULL, flags);
        }

        const unsigned char *segment = image + ph->p_offset;
        unsigned char *dest = (unsigned char *)(uintptr_t)ph->p_vaddr;
        for (uint64_t off = 0; off < ph->p_filesz; ++off) {
            dest[off] = segment[off];
        }
        for (uint64_t off = ph->p_filesz; off < ph->p_memsz; ++off) {
            dest[off] = 0;
        }
    }

    const uint64_t user_stack = 0x7FF00000ULL;
    paging_map_region(process->page_table, user_stack - 0x200000ULL, user_stack - 0x200000ULL, 0x200000ULL, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
    process->stack = user_stack;
    return 0;
}
