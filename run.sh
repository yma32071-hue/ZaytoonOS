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

# Build a writable web root that redirects / → vnc_lite.html
NOVNC_SRC="$(novnc --help 2>&1 | grep 'Default:.*webapps' | awk '{print $NF}')"
if [ -z "$NOVNC_SRC" ]; then
    NOVNC_SRC="/nix/store/n7h60i6lqysmya4clas5vghfsjc6sspa-novnc-1.6.0/share/webapps/novnc"
fi

WEB_ROOT="$(mktemp -d)"
cp -r "$NOVNC_SRC"/. "$WEB_ROOT"/

cat > "$WEB_ROOT/index.html" <<'EOF'
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="refresh" content="0; url=vnc_lite.html?autoconnect=true&reconnect=true&reconnect_delay=2000">
<title>ZaytoonOS</title>
</head>
<body>
<p>Loading ZaytoonOS...</p>
<script>window.location.replace("vnc_lite.html?autoconnect=true&reconnect=true&reconnect_delay=2000");</script>
</body>
</html>
EOF

echo "Starting noVNC on port 5000..."
exec novnc --listen 0.0.0.0:5000 --vnc localhost:5900 --web "$WEB_ROOT"
