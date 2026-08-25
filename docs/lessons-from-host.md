# Lessons from the host machine

Everything here was learned by living on one heavily-tuned Alder Lake laptop
(CachyOS, i3-1215U 2P+4E, UHD iGPU, 16G RAM, zram-only swap). These are the
empirical baselines Verdandi defaults should encode, so the OS ships with the
scars already designed out instead of requiring a decade of sysctl surgery.

## Memory and reclaim

| Host knob | Value | Lesson for Verdandi |
|---|---|---|
| transparent_hugepage | madvise | Huge pages are opt-in per address range, never auto-thump. Default: madvise-equivalent hint API. |
| vm.watermark_boost_factor | 0 | Never stall allocations to "boost" reclaim. Bursty stalls are worse than lazy watermark drift. |
| vm.watermark_scale_factor | 125 | Reclaim starts early and in the background; headroom scales with total RAM, not fixed constants. |
| dirty_bytes / dirty_background_bytes | 256M / 64M | Writeback throttling is byte-based, not ratio-based. Ratios misbehave across RAM sizes. |
| vm.page_cluster | 0 | No swap-read clustering. Swap-in is demand-paged, period. |
| vm.swappiness | 60 (+16G compressed swap) | Compressed-RAM swap is a first-class tier in the VMM, not a block-device afterthought. Anonymous-page age vs file-page age is a policy input, not a magic constant buried three layers deep. |

## Scheduling

- Schedulers are **loadable programs**, not compile-time policy. The host runs
  scx_lavd (a BPF scheduler in Rust) and swaps behavior per power state without
  rebooting. Verdandi equivalent: scheduler modules behind a stable internal
  ABI, hot-swappable, per-cgroup-class selection.
- Hybrid topology (P+E cores) must be enumerated and exposed at boot
  (CPUID 0x1A on this class of HW); runqueues are topology-aware from day one,
  not patched on later.
- CFS-era sysctls (autogroup, timeslice) were inert noise once an ext scheduler
  took over. Corollary: do not expose knobs the active scheduler ignores.
  Knobs belong to the scheduler module, not the kernel core.

## Power vs latency

- Frequency policy is HWP/EPP-first with named per-power-source profiles
  (balance_performance on AC, balance_power on battery, power on save).
  The OS ships profile presets; daemons switch them, nothing else.
- PCIe ASPM forced on, NVMe APST capped (`default_ps_max_latency_us=200`):
  deep power states are opt-out per device, and the cap exists because storage
  latency spikes are invisible in benchmarks and brutal interactively.

## Network

- fq qdisc + BBR everywhere; pacing belongs below the socket.
- Byte-based socket buffer ceilings, aggressive-but-bounded retry defaults
  (tcp_retries2 ~= 5) tuned for interactive clients, not long-haul reliability.

## Security/performance tensions (the honest list)

- `split_lock_detect=off`: bus-lock detection punishes exactly the workloads
  this machine runs. Verdandi: make it a per-task capability flag, defaulting
  to detect-and-throttle, with documented escape hatch.
- **The __memcpy_chk incident**: hardened-userland flags (`_FORTIFY_SOURCE`)
  leaked into device/JIT-compiled IR and produced unresolved `__memcpy_chk`
  in generated code. Rule: hardening applies at each codegen boundary; code
  handed to any out-of-line compiler (JIT, device toolchain) is compiled with
  its own flag set, never inherited ambient ones.
- kptr_restrict=2, ptrace_scope=1, protected_* : sane defaults, cheap to keep.

## Runtime compilation policy (owner directive)

JIT-first where specialization pays (device kernels, policy-rule evaluation,
protocol parsers), with caching that respects three budgets:

1. **Warm path**: process-local in-memory cache of compiled artifacts.
2. **Persistent path**: content-addressed on-disk store keyed by
   (source hash, target ISA, compiler version). No mtimes, no heuristics.
3. **Invalidation**: generation-counter semantics. Dropping all cached code
   for a scope is an O(1) metadata bump, never a filesystem sweep. Cache kill
   must be instant even with gigabytes compiled.

Security coupling: executable-page generation is itself capability-gated
(CAP_JIT analog, see ROADMAP M7) and always W^X - code pages are never
simultaneously writable and executable, JIT writes go through a staging map.

## Appendix: host reference values

```
cmdline: rootflags=atgc,compress=zstd:3 nowatchdog transparent_hugepage=madvise
         split_lock_detect=off intel_pstate=active,hwp_only rcutree.enable_rcu_unlazy=1
         preempt=voluntary pcie_aspm=force nvme_core.default_ps_max_latency_us=200
sysctl:  swappiness=60 vfs_cache_pressure=50 watermark_boost_factor=0
         watermark_scale_factor=125 dirty_bytes=268435456 page_cluster=0
         tcp_congestion_control=bbr default_qdisc=fq somaxconn=4096
swap:    16G zram (lz4/zstd), no disk swap
sched:   sched_ext enabled, scx_lavd autopilot
```
