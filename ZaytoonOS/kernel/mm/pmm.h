#ifndef ZAYTOONOS_MM_PMM_H
#define ZAYTOONOS_MM_PMM_H

#include "kernel/types.h"

void pmm_init(void);
void *pmm_alloc_frame(void);
void pmm_free_frame(void *frame);

#endif /* ZAYTOONOS_MM_PMM_H */
