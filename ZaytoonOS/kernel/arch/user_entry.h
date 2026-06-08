#ifndef ZAYTOONOS_ARCH_USER_ENTRY_H
#define ZAYTOONOS_ARCH_USER_ENTRY_H

#include <stdint.h>

void enter_user_mode(uint64_t entry, uint64_t stack);

#endif /* ZAYTOONOS_ARCH_USER_ENTRY_H */
