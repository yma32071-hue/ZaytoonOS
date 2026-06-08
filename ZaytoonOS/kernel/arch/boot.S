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
    
    .short 0
    .short 0
    .long 8
multiboot_header_end:

.section .text
.global _start
.extern kernel_start

_start:
    cld
    call kernel_start

_halt:
    cli
    hlt
    jmp _halt