# policy

Pure C++26 capability model and decision evaluator. The heart of the Verdandi
thesis: authorization is a total, side-effect-free function of (operation,
capability set) - which makes it fuzzable on the host with the exact source
that runs in-kernel.

- No kernel types. No allocation in `allows()`. Header-first.
- ABI note: `Cap` bit values are frozen once the kernel consumes them.

Tests: `ctest --preset host` from repo root (or standalone cmake build here).
