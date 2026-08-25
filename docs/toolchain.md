# Toolchain

## Required packages (Arch/CachyOS names)

clang, lld, llvm (llvm-objcopy etc.), qemu-system-x86, ninja, cmake >= 3.25,
ccache optional. The owner's fish config already exports the llvm binutils
(AR=llvm-ar and friends); presets pin compilers explicitly so shell env is
irrelevant - keep it that way.

## Kernel preset internals

`cmake/x86_64-kernel-toolchain.cmake` pins everything:

- `CMAKE_SYSTEM_NAME Generic` -> no libc detection attempts
- `-target x86_64-pc-none-elf -mno-red-zone -mno-mmx -mno-sse -mno-sse2`
  -> long-mode kernel code; no red zone because interrupts share the stack;
  no SSE because we do not save FPU/SIMD state yet
- `-ffreestanding -fno-builtin -nostdinc++ -fno-exceptions -fno-rtti
  -fno-stack-protector -fno-pic -fno-pie`
- links with ld.lld against kernel/linker.ld, flat image at physical 1 MiB,
  Multiboot1 header in its own section first

Why Multiboot1: `qemu-system-x86_64 -kernel file` boots it directly -
zero bootloader dependencies during the QEMU phase. Limine/UEFI arrives at
M5 (real hardware) per ROADMAP.

Boot image detail: QEMU's multiboot loader rejects ELF64 executables, so the
build post-processes verdandi.elf into verdandi.boot.elf
(`llvm-objcopy -O elf32-i386`) - same bytes, ELF32 container. Boot the
.boot.elf; debug against verdandi.elf, which keeps full symbols.

Note on `<stdint.h>`: allowed and required. With `-target *-pc-none-elf`
clang supplies it from its resource dir; `<cstdint>` is NOT usable because
`-nostdinc++` removes all C++ headers, so libvz aliases the C names into
namespace vz instead.

## Host preset

Native clang builds `policy/tests/policy_test` plus any future host-side unit
tests. This is where fuzzing will attach later (libFuzzer via `-fsanitize=fuzzer`,
host-only flag set - see lessons-from-host.md codegen-boundary rule).

## Run & debug

```sh
tools/run-qemu.sh                       # interactive, COM1 = stdio
timeout 5 qemu-system-x86_64 -m 256M \
    -kernel build/kernel/verdandi.boot.elf \
    -serial stdio -display none -no-reboot   # smoke test
```

Debug: add `-s -S`, then `lldb build/kernel/verdandi.elf` and
`gdb-remote localhost:1234`. Symbols survive because we never strip the ELF.
