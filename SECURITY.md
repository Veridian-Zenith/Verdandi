# Security policy

Verdandi is an operating system kernel. Bugs here are not crashes in an app - they are potential full-machine compromise. Reports get treated accordingly.

## How to report

Use GitHub's private security advisory for this repository (Security tab -> Report a vulnerability). If you cannot, contact the maintainer directly and ask for a private channel.

Do not open a public issue for anything that could be exploited.

## What you can expect

- Confirmation within 48 hours.
- A fix or a clear statement of why it is not being fixed, before any public disclosure.
- Credit if you want it.

## In scope

- Anything that breaks the permission model (a task doing something its capability set forbids)
- Memory corruption reachable from user code
- Boot chain issues

Out of scope right now: the host-side test tools. They never run privileged.
