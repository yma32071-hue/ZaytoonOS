.set MAGIC,    0xe85250d6
.set ARCH,     0
.set LENGTH,   multiboot_header_end - multiboot_header_start
.set CHECKSUM, -(MAGIC + ARCH + LENGTH)

.section .multiboot
.align 8
multiboot_header_start:
    .long MAGIC
    .long ARCH
    .long LENGTH
    .long CHECKSUM

    /* End tag */
    .align 8
    .short 0
    .short 0
    .long 8
multiboot_header_end:

.section .text
.global _start
.extern kernel_start

/*
 * Write a character directly to VGA text buffer at column 0
 * Uses: %eax, %edi (clobbered)
 * arg: character in %al (set before macro)
 */
.macro vga_char col, ch, color
    movl $0xB8000 + ((\col) * 2), %edi
    movb $\ch, (%edi)
    movb $\color, 1(%edi)
.endm

_start:
    cld
    /* Save multiboot magic and info pointer - GRUB puts magic in eax, info in ebx */

    /* Set up our own stack immediately */
    mov $stack_top, %esp

    /* VGA debug: write '1' at column 0 — reached _start */
    vga_char 0, '1', 0x0F

    call kernel_start

    /* VGA debug: write 'X' if kernel_start returns */
    vga_char 1, 'X', 0x4F

_halt:
    cli
    hlt
    jmp _halt

.section .bss
.align 16
stack_bottom:
    .space 16384
stack_top:
