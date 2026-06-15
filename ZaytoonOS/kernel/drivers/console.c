#include "kernel/types.h"
#include "kernel/arch/io.h"
#include "kernel/drivers/console.h"

#define VGA_TEXT_BUFFER ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_DEFAULT_COLOR 0x0A
#define TAB_WIDTH 4

/* Serial port (COM1) for debug output */
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

static uint16_t cursor_position = 0;

static void console_update_cursor(void)
{
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(cursor_position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((cursor_position >> 8) & 0xFF));
}

static void console_scroll(void)
{
    const uint32_t row_size = VGA_WIDTH;
    const uint32_t screen_size = VGA_WIDTH * VGA_HEIGHT;

    for (uint32_t i = 0; i < screen_size - row_size; ++i) {
        VGA_TEXT_BUFFER[i] = VGA_TEXT_BUFFER[i + row_size];
    }

    for (uint32_t i = screen_size - row_size; i < screen_size; ++i) {
        VGA_TEXT_BUFFER[i] = (uint16_t)(' ') | ((uint16_t)VGA_DEFAULT_COLOR << 8);
    }

    cursor_position -= row_size;
}

static void console_putc(char c)
{
    serial_putc(c);

    if (c == '\n') {
        serial_putc('\r');
        cursor_position += VGA_WIDTH - (cursor_position % VGA_WIDTH);
    } else if (c == '\r') {
        cursor_position -= cursor_position % VGA_WIDTH;
    } else if (c == '\t') {
        do {
            console_putc(' ');
        } while (cursor_position % TAB_WIDTH != 0);
        return;
    } else {
        VGA_TEXT_BUFFER[cursor_position++] = (uint16_t)c | ((uint16_t)VGA_DEFAULT_COLOR << 8);
    }

    if (cursor_position >= VGA_WIDTH * VGA_HEIGHT) {
        console_scroll();
    }

    console_update_cursor();
}

void console_init(void)
{
    serial_init();

    /* Write a bright banner to VGA to test it's working */
    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        VGA_TEXT_BUFFER[i] = (uint16_t)(' ') | ((uint16_t)0x1F << 8);
    }
    /* Reset to default color */
    for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        VGA_TEXT_BUFFER[i] = (uint16_t)(' ') | ((uint16_t)VGA_DEFAULT_COLOR << 8);
    }
    cursor_position = 0;
    console_update_cursor();
}

void console_write_char(char c)
{
    console_putc(c);
}

void console_write(const char *message)
{
    while (*message) {
        console_putc(*message++);
    }
}
