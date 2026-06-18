---
name: ZaytoonOS build and run setup
description: How ZaytoonOS is built and served; key deps and workarounds for this environment
---

## Build pipeline
- `make` in `ZaytoonOS/` → `krnl.elf` (multiboot2, 32-bit ELF)
- `grub-mkrescue` → `zaytoonos.iso`
- `qemu-system-x86_64 -cdrom zaytoonos.iso -display vnc=127.0.0.1:0` → VNC on :5900
- `novnc --listen 0.0.0.0:5000 --vnc localhost:5900 --web <writable-dir>` → port 5000

## Key CFLAGS
`-m32 -ffreestanding -mno-sse -mno-sse2 -mno-mmx -fno-builtin -fno-stack-protector -fno-pic -fno-pie`

## System deps required (nix)
`qemu`, `grub2`, `xorriso`, `novnc` — install via `installSystemDependencies()`

## Python3 not available
`embed_capps.py` was replaced with `tools/embed_capps.sh` (bash + `od`) — keeps the same output format.

## noVNC redirect
noVNC nix store is read-only; fix: `cp -r $NOVNC_SRC $WEB_ROOT`, write `index.html` redirect to `vnc_lite.html?autoconnect=true`, then `novnc --web $WEB_ROOT`.

## VGA CP437 charset
No Unicode in VGA text mode. Use ASCII only in kernel printk/console strings — em-dash `—` renders as garbage.

**Why:** VGA text mode uses CP437 which has no multi-byte UTF-8 characters.

## Keyboard driver
PS/2 polling driver (port 0x60/0x64, scancode set 1) in `kernel/drivers/keyboard.c`. Shift + caps-lock tracking. `keyboard_getchar()` is blocking spin-poll; timer IRQ preempts it via the scheduler.

## capp context readchar
`capp_context_t` has a `readchar` callback set to `keyboard_getchar` — native capps can use `ctx->readchar()` for interactive input.
