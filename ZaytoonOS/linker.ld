OUTPUT_FORMAT("elf64-x86-64")
ENTRY(kernel_start)

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x100000;

    .text : {
        KEEP(*(.multiboot)) 
        *(.text*)
    } :text

    .rodata : {
        *(.rodata*)
    } :text

    .data : {
        *(.data*)
    } :data

    .bss : {
        *(.bss*)
        *(COMMON)
    } :data

    _end = .;
    . = ALIGN(0x1000);
    _heap_start = .;
    . += 0x00100000;
    _heap_end = .;
}