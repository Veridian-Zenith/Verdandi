#pragma once
#include <stdint.h>

#include "vz/types.hpp"

namespace arch {

inline void out8(vz::u16 port, vz::u8 value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

inline vz::u8 in8(vz::u16 port) {
    vz::u8 v;
    asm volatile("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

} // namespace arch
