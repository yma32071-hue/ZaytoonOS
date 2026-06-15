#!/usr/bin/env bash
set -e

cd "$(dirname "$0")/ZaytoonOS"

echo "Building ZaytoonOS kernel..."
make

echo "Copying kernel to ISO directory..."
cp krnl.elf ../iso/boot/krnl.elf

echo "Building bootable ISO..."
grub-mkrescue -o ../zaytoonos.iso ../iso 2>&1

echo "Starting ZaytoonOS in QEMU (VNC on :5900)..."
qemu-system-x86_64 \
    -machine accel=tcg \
    -cdrom ../zaytoonos.iso \
    -boot d \
    -display vnc=127.0.0.1:0 \
    -serial stdio \
    -no-reboot &
QEMU_PID=$!

echo "Waiting for QEMU VNC to start..."
sleep 4

echo "Starting noVNC on port 5000..."
exec novnc --listen 0.0.0.0:5000 --vnc localhost:5900
