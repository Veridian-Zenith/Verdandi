#include "arch/x86_64/apic.hpp"

#include "arch/x86_64/io.hpp"

namespace arch {

namespace {

void out32(vz::u16 port, vz::u32 value) {
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

vz::u64 rdmsr(vz::u32 index) {
    vz::u32 lo;
    vz::u32 hi;
    asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(index));
    return (static_cast<vz::u64>(hi) << 32) | lo;
}

void wrmsr(vz::u32 index, vz::u64 value) {
    asm volatile("wrmsr"
                 :
                 : "a"(static_cast<vz::u32>(value)), "d"(static_cast<vz::u32>(value >> 32)),
                   "c"(index));
}

constexpr vz::u32 kApicBaseMsr = 0x1B;

} // namespace

void mask_legacy_pic() {
    out8(0x20, 0x11); /* ICW1: init, cascade */
    out8(0xA0, 0x11);
    out8(0x21, 0x20); /* ICW2: would-be vectors (unused, lines stay masked) */
    out8(0xA1, 0x28);
    out8(0x21, 0x04); /* ICW3 */
    out8(0xA1, 0x02);
    out8(0x21, 0x01); /* ICW4: 8086 mode */
    out8(0xA1, 0x01);
    out8(0x21, 0xFF); /* OCW1: mask everything */
    out8(0xA1, 0xFF);
    out32(0x20, 0x20); /* spurious EOI just in case */
    out32(0xA0, 0x20);
}

bool Lapic::probe() {
    vz::u32 a;
    vz::u32 b;
    vz::u32 c;
    vz::u32 d;
    asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(1u));
    return (d & (1u << 9)) != 0; /* EDX bit 9: APIC present */
}

void Lapic::enable() {
    vz::u64 msr = rdmsr(kApicBaseMsr);
    base_ = static_cast<vz::u32>(msr & 0xFFFFF000ull);
    msr |= 1ull << 11;  /* APIC global enable */
    msr &= ~(1ull << 10); /* xAPIC (MMIO) mode, not x2APIC */
    wrmsr(kApicBaseMsr, msr);

    write(0xF0, read(0xF0) | 0x100 | 0xFF); /* spurious: vector FF, enabled */
}

void Lapic::start_timer(vz::u32 initial_count) {
    write(0x3E0, 3);                      /* divide config: /16 */
    write(0x320, kTimerVector | (1u << 17)); /* LVT: periodic, unmasked */
    write(0x380, initial_count);          /* starts counting */
}

void Lapic::stop_timer() {
    write(0x320, kTimerVector | (1u << 16)); /* LVT masked */
    write(0x380, 0);
}

void Lapic::send_eoi() {
    write(0xB0, 0);
}

vz::u32 Lapic::read(vz::u32 offset) const {
    return *reinterpret_cast<volatile vz::u32*>(base_ + offset);
}

void Lapic::write(vz::u32 offset, vz::u32 value) {
    *reinterpret_cast<volatile vz::u32*>(base_ + offset) = value;
}

} // namespace arch
