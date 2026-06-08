#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void console_init(void);
void console_write_char(char c);
void console_write(const char *message);

#endif /* CONSOLE_H */
