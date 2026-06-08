#include "kernel/irq/pic.h"
#include "kernel/arch/io.h"
#include "kernel/kernel/printk.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define ICW1_INIT    0x11
#define ICW4_8086    0x01

void pic_remap(void)
{
    u8 a1 = inb(PIC1_DATA);
    u8 a2 = inb(PIC2_DATA);
    (void)a1;
    (void)a2;

    outb(PIC1_COMMAND, ICW1_INIT);
    outb(PIC2_COMMAND, ICW1_INIT);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
    printk("[irq] PIC remapped");
}

void pic_mask(u8 irq_line)
{
    u16 port = irq_line < 8 ? PIC1_DATA : PIC2_DATA;
    u8 value = inb(port) | (1 << (irq_line % 8));
    outb(port, value);
}

void pic_unmask(u8 irq_line)
{
    u16 port = irq_line < 8 ? PIC1_DATA : PIC2_DATA;
    u8 value = inb(port) & ~(1 << (irq_line % 8));
    outb(port, value);
}

void pic_eoi(u8 irq)
{
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}
