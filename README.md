# Verdandi

> Provisional name (Norn of "that which is becoming"). Rename freely; see docs/architecture.md.

A capability-policy-native operating system for x86_64, written in freestanding C++20 on the LLVM toolchain.

Thesis: on Linux, privilege enforcement is bolted on after process birth (seccomp filters, LSM hooks, brokers like Voix). Verdandi makes policy evaluation part of process creation itself: every task is born from a declarative capability set, and the kernel mediates all capability-gated operations at the only point that cannot be bypassed - the syscall boundary it implements.

Status: Phase 0 (skeleton). See ROADMAP.md for milestone gates and docs/toolchain.md to build.

## Layout

| Path        | Future repo? | Purpose |
|-------------|--------------|---------|
| `kernel/`   | core, stays  | Kernel (freestanding): arch code, mm, sched, syscalls |
| `libvz/`    | maybe        | Shared freestanding C++ runtime bits (no libc assumed) |
| `policy/`   | yes          | Capability model + evaluator. Pure C++, host-testable, no kernel deps |
| `init/`     | yes          | PID 1. Successor to the Galdr init design |
| `tools/`    | yes          | Host-side dev tools (run scripts, image packing) |
| `docs/`     | stays        | Architecture decisions, toolchain guide, roadmap |

Extraction rules live in docs/architecture.md. In short: siblings may only depend on each other through public headers under `include/`, every component carries its own README and is buildable standalone, so any directory can become its own repository with `git filter-repo` when it earns one.
