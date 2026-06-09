#ifndef ZAYTOONOS_KERNEL_SHELL_STRING_H
#define ZAYTOONOS_KERNEL_SHELL_STRING_H

#include "kernel/types.h"

bool shell_streq_ci(const char *a, const char *b);
bool shell_starts_with_ci(const char *text, const char *prefix);
u32 shell_strlen(const char *text);
void shell_copy(char *dst, u32 dst_size, const char *src);
void shell_format_u64(char *dst, u32 dst_size, const char *prefix, u64 value);

#endif /* ZAYTOONOS_KERNEL_SHELL_STRING_H */
