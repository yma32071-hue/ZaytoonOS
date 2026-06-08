#ifndef ZAYTOONOS_MM_PAGING_H
#define ZAYTOONOS_MM_PAGING_H

#include <stdint.h>

void paging_init(void);
uint64_t *paging_create_user_space(void);
void paging_map_region(uint64_t *pml4, uint64_t virt, uint64_t phys, uint64_t size, uint64_t flags);

#endif /* ZAYTOONOS_MM_PAGING_H */
