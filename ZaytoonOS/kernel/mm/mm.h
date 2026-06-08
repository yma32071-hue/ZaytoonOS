#ifndef MM_H
#define MM_H

#include <stddef.h>

void mm_init(void);
void *kmalloc(size_t size);
void *kmalloc_aligned(size_t size, size_t align);

#endif /* MM_H */
