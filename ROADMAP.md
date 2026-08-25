# Verdandi milestones

Each milestone has an acceptance gate. Do not start M(n+1) before M(n) passes.
All work happens in QEMU until M5.

- [x] M0 Skeleton builds
      Gate: `cmake --preset kernel && cmake --build --preset kernel` produces
      build/kernel/verdandi.elf; `tools/run-qemu.sh` prints the banner on COM1.
- [ ] M1 CPU alive
      GDT, IDT, ISRs with full register dump on fault, PIC->APIC timer tick,
      serial console driver. Gate: divide-by-zero handler prints regs and recovers.
- [ ] M2 Physical memory
      Multiboot2 memory map -> frame allocator (bitmap or buddy). Gate: alloc /
      free 10k frames under stress test without leak or double-free.
- [ ] M3 Virtual memory
      Higher-half kernel mapping, kmalloc family, per-process page tables.
      Gate: two address spaces switch via context switch without corruption.
- [ ] M4 Preemption + SMP
      LAPIC timer preemption, boot APs, enumerate hybrid topology via CPUID
      0x1A (P vs E cores). Gate: N runnable tasks migrate across all 8 threads;
      topology printed matches lscpu of the host machine.
- [ ] M5 Real hardware
      Boot the i3-1215U laptop: GOP framebuffer, NVMe read-only, xHCI later.
      Gate: banner on the physical panel from NVMe-loaded image.
- [ ] M6 ELF + init
      ELF64 loader, first user process = init. Gate: user task makes a syscall
      and returns without trashing the kernel.
- [ ] M7 Policy core lands
      policy/ evaluator wired into fork/exec: every new task gets a capability
      set; syscalls check it. Gate: task lacking CAP_FS_WRITE cannot open(O_WRONLY),
      positive and negative tests pass on both host unit tests and kernel.
- [ ] M8 Two processes talk
      Kernel-mediated message channel with per-direction capability checks.
      Gate: sender without send-cap gets EPERM; receiver gets exactly the bytes.

Stretch (post-M8): ext-family or tarfs, xHCI storage, Voix as userspace policy
compiler emitting kernel policy blobs, SMP scheduler experiments for hybrid
topology (the scx_lavd knowledge, reimplemented from scratch).

## Tooling backlog

Running today (GitHub Actions):
- ci.yml: kernel build + QEMU boot smoke, host tests, ASan+UBSan pass,
  clang-format gate
- codeql.yml: weekly + per-push C++ analysis over both build targets

Queued, in the order they earn their keep:
- libFuzzer target for policy::allows / future rule parser (host-only; pairs
  with SECURITY scope) - add once the evaluator grows beyond a bitmask
- clang-tidy gate on policy/ and libvz/ only - kernel code gets too noisy too early
- coverage report for host tests (llvm-cov), surfaced as a badge
- kernel-side hardening checks once M3 lands: guard pages for kmalloc,
  poison-on-free, then a KASAN-style shadow when paging is ours to control
