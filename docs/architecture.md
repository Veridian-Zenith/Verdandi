# Architecture

## Components and dependency direction

```
        +-------------------------------------------------+
        |                    kernel                       |
        |  arch/x86_64 | mm | sched | syscall | policy-if |
        +----^-----------------------^--------------------+
             |                       |
        libvz (freestanding)     policy (pure C++, host-testable)
             |                       |
        init  -> uses both via public headers
```

Dependency rules (violations are build errors, not style advice):

1. `kernel`, `init` may use `libvz` and `policy`.
2. `policy` may NOT include kernel headers. It is a pure library: same source
   compiles on host glibc/clang for unit tests and freestanding in the kernel.
3. `libvz` may not depend on any sibling. It is the bottom layer.
4. Cross-component includes go through `<component/public_header.hpp>` only -
   never relative paths into another component's src/.

## Repo-extraction contract

Every top-level directory is a future standalone repository:

- own `README.md` stating purpose and status
- own `CMakeLists.txt` with a guard so it builds standalone
  (`if(NOT PROJECT_NAME) project(policy ...) endif()` pattern)
- zero relative includes across siblings
- its tests live inside it (`policy/tests/`)

When one earns its own history: `git filter-repo --subdirectory-filter policy`
into a fresh repo; nothing else in the tree needs to change.

## Kernel internals map

```
kernel/
  arch/x86_64/    entry, GDT/IDT, APIC, CPUID topology, page table code
  mm/             frame alloc, kmalloc, address spaces
  sched/          runqueues (hybrid-topology aware from M4), context switch
  syscall/        dispatch table; every entry is capability-checked (M7+)
  drivers/        serial, GOP, NVMe, xHCI (each one file per device class)
  policy-if/      glue: kernel types <-> policy::CapabilitySet
```

## Decisions log

Append-only; newest last. Format: date - decision - reason.

- 2026-08-25 - Freestanding C++20, LLVM/Clang toolchain, no libc, no Rust.
  Owner preference: heavy optimization control and the LLVM ecosystem.
- 2026-08-25 - Multiboot1 boot protocol for QEMU phase; Limine/UEFI upgrade
  planned at M5. Multiboot1 boots via plain `qemu -kernel`, zero extra deps.
- 2026-08-25 - Capability model lives in `policy/` as a pure library so the
  evaluator can be fuzzed on host with the exact bytes that run in-kernel.
- 2026-08-25 - Language standard pinned to C++26. Owner directive; clang 22 supports it fully for our freestanding subset.
- 2026-08-25 - Shell env (CFLAGS etc.) is reset inside CMakeLists/toolchain files, never in presets alone: kernel pins everything in cmake/i686-kernel-toolchain.cmake (CACHE FORCE), host resets then applies -march=native -mtune=native in the root listfile. Presets stay minimal.
- 2026-08-25 - M0 verified end-to-end: verdandi.elf boots under qemu-system-x86_64 -kernel, banner on COM1; policy host tests pass under ctest.
- 2026-08-25 - No file-scope statics in our code. Drivers are instances (SerialPort(base)), console output routes through a runtime-registered sink; kmain constructs and injects dependencies. One deliberate root: the console sink slot. ABI-frozen items (capability bit values) stay static by design.
- 2026-08-25 - Long-mode entry stays on Multiboot1 for QEMU phase: entry.S pages itself into long mode (2 MiB identity map over the full 4 GiB so LAPIC MMIO at 0xFEE00000 needs no special case until M3 paging). QEMU -kernel only loads ELF32, so the build converts linked ELF64 to verdandi.boot.elf via llvm-objcopy; symbols live in verdandi.elf for lldb.
- 2026-08-25 - Bootstrap GDT selector layout (null, code64@0x08, data@0x10) is a frozen contract with arch::Gdt::set_flat_lm: installing the runtime GDT reloads data segments only, no far jump.
- 2026-08-25 - Second deliberate runtime-registration root: Idt::install() publishes g_active_idt (same pattern as the console sink) because asm stubs carry no instance context. Roots are allowed only where a hardware entry point has nowhere to carry a pointer.
