#include <cassert>
#include <cstdio>
#include "policy/capability.hpp"

using namespace policy;

static int failures = 0;
#define CHECK(cond)                                                     \
    do { if (!(cond)) { ++failures;                                     \
        std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } } \
    while (0)

int main() {
    auto s = CapabilitySet::none();
    CHECK(!allows(Op::Spawn, s));
    CHECK(!allows(Op::OpenWrite, CapabilitySet::all().revoke(Cap::FileWrite)));

    s.grant(Cap::FileRead);
    CHECK(allows(Op::OpenRead, s));
    CHECK(!allows(Op::OpenRead, CapabilitySet::none()));
    CHECK(s.raw() == static_cast<uint64_t>(Cap::FileRead));

    auto jit = CapabilitySet::none();
    jit.grant(Cap::ExecJit);
    CHECK(allows(Op::GenerateCode, jit));
    CHECK(!allows(Op::OpenDevice, jit));

    if (failures == 0) { std::printf("policy tests: all pass\n"); return 0; }
    std::printf("policy tests: %d failure(s)\n", failures);
    return 1;
}
