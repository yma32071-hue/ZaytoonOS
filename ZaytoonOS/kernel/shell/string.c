#include "kernel/shell/string.h"

static char lower_ascii(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (char)(c + ('a' - 'A'));
    }
    return c;
}

bool shell_streq_ci(const char *a, const char *b)
{
    if (!a || !b) {
        return false;
    }

    while (*a && *b) {
        if (lower_ascii(*a++) != lower_ascii(*b++)) {
            return false;
        }
    }
    return *a == '\0' && *b == '\0';
}

bool shell_starts_with_ci(const char *text, const char *prefix)
{
    if (!text || !prefix) {
        return false;
    }

    while (*prefix) {
        if (lower_ascii(*text++) != lower_ascii(*prefix++)) {
            return false;
        }
    }
    return true;
}

u32 shell_strlen(const char *text)
{
    u32 len = 0;
    while (text && text[len]) {
        ++len;
    }
    return len;
}

void shell_copy(char *dst, u32 dst_size, const char *src)
{
    if (!dst || dst_size == 0) {
        return;
    }

    u32 i = 0;
    while (src && src[i] && i + 1 < dst_size) {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = '\0';
}

void shell_format_u64(char *dst, u32 dst_size, const char *prefix, u64 value)
{
    if (!dst || dst_size == 0) {
        return;
    }

    u32 pos = 0;
    while (prefix && *prefix && pos + 1 < dst_size) {
        dst[pos++] = *prefix++;
    }

    char digits[21];
    u32 digit_count = 0;
    if (value == 0) {
        digits[digit_count++] = '0';
    } else {
        while (value && digit_count < sizeof(digits)) {
            digits[digit_count++] = (char)('0' + (value % 10));
            value /= 10;
        }
    }

    while (digit_count && pos + 1 < dst_size) {
        dst[pos++] = digits[--digit_count];
    }
    dst[pos] = '\0';
}
