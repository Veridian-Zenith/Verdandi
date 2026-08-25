# Contributing

Thanks for interest. The rules here are short on purpose.

## Before you write code

- Open an issue first for anything bigger than a typo fix. Say what you want to change and why. Big surprise pull requests get closed.
- One idea per pull request. If your PR fixes three things, it is three PRs.
- Read [docs/architecture.md](docs/architecture.md). Two rules there are enforced by review, not just style:
  - components only talk to each other through their public headers
  - anything that changes a design decision gets a dated line in the decisions log

## Making sure it works

Run all of these before opening your PR:

```sh
cmake --build --preset kernel     # must build with zero warnings
ctest --preset host               # must pass
tools/run-qemu.sh                 # boot banner must still appear
```

If you touched boot behavior, say so in the PR and paste what QEMU printed.

## Style

- `clang-format` config is in `.clang-format` - run it, do not debate it.
- Comments only where the code cannot speak. Short sentences.
- No new dependencies without an issue discussing it first. The dependency list is intentionally tiny: clang/lld/cmake/ninja/qemu.

## Reporting problems

Bugs with a working build: normal issue, include the exact commands you ran and their output.

Security holes: [SECURITY.md](SECURITY.md), private channel, not issues.

## Licensing

By contributing you agree your work is released under the project license (OSL-3.0). That is the whole agreement.
