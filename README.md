# ZaytoonOS

A minimalist x86_64 kernel with a modular runtime architecture and basic preemptive scheduling.

This version adds a stronger kernel foundation:

- formatted `printk` logging with `%s`, `%d`, `%x`, `%p`, and more
- VGA text console with scrolling, cursor updates, and tab handling
- a kernel heap allocator based on linker-provided heap symbols
- PIC remapping and timer-driven IRQ scheduling
- a lightweight task subsystem with context switching
- improved build flags for bare-metal compilation

## Build

From the `ZaytoonOS` directory:

    make

This produces `zaytoonos.elf` from the kernel and root entry point.

## Run

From the `ZaytoonOS` directory:

    make run

## noVNC

A local noVNC setup is available in `novnc/`.

To launch a virtual X desktop and expose it through noVNC:

    cd novnc
    ./start_novnc.sh

Then open the forwarded browser URL for port `6080`.

If your workspace requires a specific VNC target instead of the built-in virtual desktop, use:

    cd novnc
    ./utils/novnc_proxy --vnc localhost:5901 --listen 6080

## Structure

- `kernel/` — kernel subsystem sources
- `maincompiler.c` — root startup wrapper that begins kernel execution
- `Makefile` — build rules for the OS image
- `linker.ld` — bare-metal linker layout for the kernel

## NOTE!!1!
if you want to compile the .elf into an iso first put it in the boot folder in iso (NOT IN THE GRUB FOLDER) then run

```bash
grub-mkrescue -o /workspaces/ZaytoonOS/zaytoonos.iso /workspaces/ZaytoonOS/iso

```
IF you have ```bash 

            grub-common
installed on linux, if you want to use windows, install WSL in powershell from ```bash 

                                                                                  wsl -install
                                                                                  
