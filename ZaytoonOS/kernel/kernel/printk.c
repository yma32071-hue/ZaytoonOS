#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include "kernel/kernel/printk.h"
#include "kernel/drivers/console.h"

static char *format_unsigned(char *buffer, unsigned long value, unsigned int base, bool uppercase)
{
    static const char digits_lower[] = "0123456789abcdef";
    static const char digits_upper[] = "0123456789ABCDEF";
    const char *digits = uppercase ? digits_upper : digits_lower;
    char temp[32];
    char *ptr = temp;

    if (value == 0) {
        *buffer++ = '0';
        return buffer;
    }

    while (value != 0) {
        *ptr++ = digits[value % base];
        value /= base;
    }

    while (ptr != temp) {
        *buffer++ = *--ptr;
    }

    return buffer;
}

static size_t kvsnprintf(char *buffer, size_t size, const char *format, va_list args)
{
    char *ptr = buffer;
    const char *end = buffer + size - 1;

    while (*format && ptr < end) {
        if (*format != '%') {
            *ptr++ = *format++;
            continue;
        }

        format++;
        bool long_mode = false;
        if (*format == 'l') {
            long_mode = true;
            format++;
        }

        switch (*format++) {
        case 'c':
            if (ptr < end) {
                *ptr++ = (char)va_arg(args, int);
            }
            break;
        case 's': {
            const char *value = va_arg(args, const char *);
            while (*value && ptr < end) {
                *ptr++ = *value++;
            }
        } break;
        case 'u': {
            unsigned long value = long_mode ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
            ptr = format_unsigned(ptr, value, 10, false);
        } break;
        case 'd': {
            long value = long_mode ? va_arg(args, long) : va_arg(args, int);
            if (value < 0) {
                if (ptr < end) {
                    *ptr++ = '-';
                }
                value = -value;
            }
            ptr = format_unsigned(ptr, (unsigned long)value, 10, false);
        } break;
        case 'x':
        case 'p': {
            unsigned long value = long_mode ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
            ptr = format_unsigned(ptr, value, 16, false);
        } break;
        case 'X': {
            unsigned long value = long_mode ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
            ptr = format_unsigned(ptr, value, 16, true);
        } break;
        case '%':
            if (ptr < end) {
                *ptr++ = '%';
            }
            break;
        default:
            if (ptr < end) {
                *ptr++ = '%';
            }
            if (ptr < end) {
                *ptr++ = format[-1];
            }
            break;
        }
    }

    *ptr = '\0';
    return ptr - buffer;
}

void printk(const char *format, ...)
{
    char output[256];
    va_list args;
    va_start(args, format);
    kvsnprintf(output, sizeof(output), format, args);
    va_end(args);

    console_write(output);
    console_write("\n");
}
