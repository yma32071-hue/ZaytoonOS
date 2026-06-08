#ifndef ZAYTOONOS_USER_ELF_LOADER_H
#define ZAYTOONOS_USER_ELF_LOADER_H

#include "kernel/types.h"

struct elf_process {
    u64 entry;
    u64 stack;
    uint64_t *page_table;
};

int elf_load_image(const unsigned char *image, size_t size, struct elf_process *process);

#endif /* ZAYTOONOS_USER_ELF_LOADER_H */
