#pragma once
#include <stdint.h>

namespace policy {

// Capability bits. Values are ABI-stable: they appear in kernel policy blobs.
enum class Cap : uint64_t {
    ProcessSpawn = 1ull << 0,
    FileRead = 1ull << 1,
    FileWrite = 1ull << 2,
    NetBind = 1ull << 3,
    NetConnect = 1ull << 4,
    ExecJit = 1ull << 5, // runtime code generation (see lessons-from-host.md)
    DeviceOpen = 1ull << 6,
};

class CapabilitySet {
  public:
    constexpr CapabilitySet() = default;
    static constexpr CapabilitySet none() { return {}; }
    static constexpr CapabilitySet all() { return CapabilitySet{0xFFFFFFFFFFFFFFFFull}; }

    constexpr bool has(Cap c) const { return bits_ & static_cast<uint64_t>(c); }
    constexpr CapabilitySet& grant(Cap c) {
        bits_ |= static_cast<uint64_t>(c);
        return *this;
    }
    constexpr CapabilitySet& revoke(Cap c) {
        bits_ &= ~static_cast<uint64_t>(c);
        return *this;
    }
    constexpr uint64_t raw() const { return bits_; }

  private:
    explicit constexpr CapabilitySet(uint64_t b) : bits_(b) {}
    uint64_t bits_ = 0;
};

enum class Op {
    Spawn,
    OpenRead,
    OpenWrite,
    BindListen,
    OutboundConnect,
    GenerateCode,
    OpenDevice,
};

// Pure decision function. No side effects, no allocation - the exact property
// that lets host fuzzing stand in for in-kernel testing.
constexpr bool allows(Op op, const CapabilitySet& s) {
    switch (op) {
    case Op::Spawn:
        return s.has(Cap::ProcessSpawn);
    case Op::OpenRead:
        return s.has(Cap::FileRead);
    case Op::OpenWrite:
        return s.has(Cap::FileWrite);
    case Op::BindListen:
        return s.has(Cap::NetBind);
    case Op::OutboundConnect:
        return s.has(Cap::NetConnect);
    case Op::GenerateCode:
        return s.has(Cap::ExecJit);
    case Op::OpenDevice:
        return s.has(Cap::DeviceOpen);
    }
    return false;
}

} // namespace policy
