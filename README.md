# Verdandi

An operating system for x86_64 computers, built from scratch.

Most hobby operating systems are "Unix, but smaller." Verdandi starts from a different question: what if permission to do things was part of the system's core, not a patch on top? Here, every program is born holding an explicit list of what it may do - read files, use the network, generate code. The kernel checks that list itself. There is no layer of security tools stacked after the fact, because the decision point and the kernel are the same thing.

Status: very early. It prints to a serial port and halts. That is honest progress - see [ROADMAP.md](ROADMAP.md) for the full ladder.

## Try it

You need clang, lld, cmake, ninja, and qemu (see [docs/toolchain.md](docs/toolchain.md)).

```sh
cmake --preset kernel
cmake --build --preset kernel
tools/run-qemu.sh
```

You should see:

```
[verdandi] phase 2 alive: long mode, COM1 up
[verdandi] lapic timer: 10 ticks observed
[verdandi] recovered from #DE: M1 gate passed
```

## How it is organized

| Folder | What lives there |
|---|---|
| `kernel/` | the operating system itself |
| `policy/` | the permission model - plain code, testable on any machine |
| `libvz/` | small shared pieces used by everything |
| `init/` | first userspace program (arrives at milestone 6) |
| `tools/` | helper scripts for building and running |
| `docs/` | how it works and why |

Two documents explain most decisions:
[docs/architecture.md](docs/architecture.md) (how the pieces fit)
and [docs/lessons-from-host.md](docs/lessons-from-host.md) (why the defaults look the way they do).

## Want to help?

Read [CONTRIBUTING.md](CONTRIBUTING.md). Small, focused changes with working builds are accepted fastest. Security issues go through [SECURITY.md](SECURITY.md), never public issues.

## License

[OSL-3.0](LICENSE). Short version: use it freely, including commercially; if you publish or run it for others, your changes stay under this license; you cannot use our name to endorse your fork. The file has the exact terms.
