---
name: ZaytoonOS boot fixes
description: Root causes and fixes that got the 32-bit x86 kernel booting under GRUB2/QEMU/noVNC on Replit
---

# ZaytoonOS Boot Fix Summary

**Why:** The kernel was written targeting x86-64 but compiled as 32-bit (-m32), causing multiple crash categories.

## Fix 1: ELF entry point — no stack before first C call
- **Problem:** `linker.ld` had `ENTRY(kernel_start)` so GRUB jumped directly to C with no stack.
- **Fix:** Changed to `ENTRY(_start)`; `boot.S` sets `esp = stack_top` before calling `kernel_start`.
- **How to apply:** Always make the asm `_start` the ELF entry for multiboot kernels.

## Fix 2: 64-bit structs/MSRs in 32-bit mode
- **Problem:** `idt.c` used 16-byte IDT entries (64-bit format); `tss.c` used 64-bit TSS struct; `arch.c` called `setup_syscall()` writing EFER/STAR/LSTAR (64-bit only MSRs); `paging.c` used PML4.
- **Fix:** Rewrote with 8-byte 32-bit IDT entries, `struct tss32_entry`, removed `setup_syscall()`, stubbed `paging_init()`.

## Fix 3: SSE instructions emitted by GCC -O2 for loop zeroing
- **Problem:** GCC auto-vectorized `irq_table[256] = {0}` into `pxor %xmm0` (SSE2). Without CR4.OSFXSR, this is #UD → triple fault.
- **Fix:** Added `-mno-sse -mno-sse2 -mno-mmx` to CFLAGS in Makefile.
- **Why:** Bare-metal kernels must disable SSE at compile time.

## Fix 4: Premature printk before console_init
- **Problem:** `irq.c` and `pic.c` called `printk()` before `console_init()` — `serial_putc()` spun on uninitialized UART.
- **Fix:** Removed all `printk` from `pic_remap()` and `irq_init()`.

## Fix 5: gdt_set_tss signature
- **Fix:** Changed `gdt_set_tss(uint64_t base, uint32_t limit)` → `gdt_set_tss(u32 base, u32 limit)`.
