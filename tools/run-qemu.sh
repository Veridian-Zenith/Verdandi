#!/bin/sh
# Boot the kernel in QEMU with COM1 on stdio. Ctrl-C then `quit` exits monitor.
set -e
ELF="$(dirname "$0")/../build/kernel/verdandi.boot.elf"
[ -f "$ELF" ] || { echo "missing $ELF - run: cmake --preset kernel && cmake --build --preset kernel" >&2; exit 1; }
exec qemu-system-x86_64 \
    -m 256M \
    -kernel "$ELF" \
    -serial stdio \
    -display none \
    -no-reboot
