#include "arch/x86_64/gdt.hpp"

namespace arch {

void Gdt::set_flat_lm() {
    entries_[0] = 0;
    entries_[kSelCode / 8] = 0x00AF9A000000FFFFull; /* code: P ring0 L G */
    entries_[kSelData / 8] = 0x00CF92000000FFFFull; /* data: P ring0 W G */
}

void Gdt::install() {
    struct {
        vz::u16 limit;
        vz::u64 base;
    } __attribute__((packed)) gdtr{sizeof(entries_) - 1,
                                   reinterpret_cast<vz::u64>(entries_)};

    asm volatile("lgdt %0" : : "m"(gdtr));
    asm volatile("mov %0, %%ds" : : "r"(kSelData));
    asm volatile("mov %0, %%es" : : "r"(kSelData));
    asm volatile("mov %0, %%fs" : : "r"(kSelData));
    asm volatile("mov %0, %%gs" : : "r"(kSelData));
    asm volatile("mov %0, %%ss" : : "r"(kSelData));
}

} // namespace arch
