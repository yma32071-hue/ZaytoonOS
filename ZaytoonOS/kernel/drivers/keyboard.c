#include "kernel/drivers/keyboard.h"
#include "kernel/arch/io.h"

#define KBD_DATA   0x60
#define KBD_STATUS 0x64

static int shift_held = 0;
static int caps_lock  = 0;

/* US QWERTY scancode set 1 → ASCII, 128 entries (index = scancode) */
static const char sc_lower[] = {
/*00*/  0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',  '\b', '\t',
/*10*/ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
/*1E*/ 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
/*2B*/ '\\','z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
/*37*/ '*',  0,  ' ',  0,
/*3B*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*54*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*70*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                           /* 0x70..0x7F */
};

static const char sc_upper[] = {
/*00*/  0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',  '\b', '\t',
/*10*/ 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
/*1E*/ 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
/*2B*/ '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',  0,
/*37*/ '*',  0,  ' ',  0,
/*3B*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*54*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*70*/  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                           /* 0x70..0x7F */
};

void keyboard_init(void)
{
    shift_held = 0;
    caps_lock  = 0;
}

int keyboard_poll_char(void)
{
    if (!(inb(KBD_STATUS) & 0x01)) {
        return -1;
    }

    unsigned char sc = inb(KBD_DATA);

    if (sc & 0x80) {
        unsigned char rel = sc & 0x7F;
        if (rel == 0x2A || rel == 0x36) {
            shift_held = 0;
        }
        return -1;
    }

    if (sc == 0x2A || sc == 0x36) {
        shift_held = 1;
        return -1;
    }
    if (sc == 0x3A) {
        caps_lock = !caps_lock;
        return -1;
    }

    if (sc >= (unsigned char)sizeof(sc_lower)) {
        return -1;
    }

    int use_upper = shift_held ^ (caps_lock && sc >= 0x10 && sc <= 0x32);
    char c = use_upper ? sc_upper[sc] : sc_lower[sc];
    return c ? (int)(unsigned char)c : -1;
}

char keyboard_getchar(void)
{
    for (;;) {
        int c = keyboard_poll_char();
        if (c > 0) {
            return (char)c;
        }
    }
}
