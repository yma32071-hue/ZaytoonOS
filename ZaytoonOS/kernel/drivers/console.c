#include "kernel/types.h"
#include "kernel/arch/io.h"
#include "kernel/drivers/console.h"

#define VGA_TEXT_BUFFER ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_COLOR  0x0A
#define TAB_WIDTH  4

#define SERIAL_PORT 0x3F8

static void serial_init(void)
{
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT + 0, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);
    outb(SERIAL_PORT + 2, 0xC7);
    outb(SERIAL_PORT + 4, 0x0B);
}

static void serial_putc(char c)
{
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0) {}
    outb(SERIAL_PORT, (uint8_t)c);
}

static uint16_t cursor_pos = 0;

static void update_cursor(void)
{
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(cursor_pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((cursor_pos >> 8) & 0xFF));
}

static void scroll_up(void)
{
    for (uint32_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); ++i) {
        VGA_TEXT_BUFFER[i] = VGA_TEXT_BUFFER[i + VGA_WIDTH];
    }
    for (uint32_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        VGA_TEXT_BUFFER[i] = (uint16_t)' ' | ((uint16_t)VGA_COLOR << 8);
    }
    cursor_pos -= VGA_WIDTH;
}

static void console_putc(char c)
{
    serial_putc(c);

    if (c == '\n') {
        serial_putc('\r');
        cursor_pos += VGA_WIDTH - (cursor_pos % VGA_WIDTH);
    } else if (c == '\r') {
        cursor_pos -= cursor_pos % VGA_WIDTH;
    } else if (c == '\b') {
        if (cursor_pos > 0) {
            cursor_pos--;
            VGA_TEXT_BUFFER[cursor_pos] = (uint16_t)' ' | ((uint16_t)VGA_COLOR << 8);
            update_cursor();
        }
        return;
    } else if (c == '\t') {
        do { console_putc(' '); } while (cursor_pos % TAB_WIDTH != 0);
        return;
    } else {
        VGA_TEXT_BUFFER[cursor_pos++] = (uint16_t)(unsigned char)c | ((uint16_t)VGA_COLOR << 8);
    }

    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        scroll_up();
    }

    update_cursor();
}

void console_init(void)
{
    serial_init();
    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        VGA_TEXT_BUFFER[i] = (uint16_t)' ' | ((uint16_t)VGA_COLOR << 8);
    }
    cursor_pos = 0;
    update_cursor();
}

void console_write_char(char c)
{
    console_putc(c);
}

void console_write(const char *s)
{
    while (s && *s) {
        console_putc(*s++);
    }
}
