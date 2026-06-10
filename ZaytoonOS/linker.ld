ENTRY(kernel_start)
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
    /* Begin loading kernel at the 1MB mark */
    . = 0x100000;

    .text : ALIGN(4K) {
        KEEP(*(.multiboot))
        *(.text*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata*)
    }

    .data : ALIGN(4K) {
        *(.data*)
    }

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss*)
    }

    _end = .;
    . = ALIGN(0x1000);
    _heap_start = .;
    . += 0x00100000;
    _heap_end = .;
}