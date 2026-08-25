#pragma once
#include <stdint.h>

#include "vz/types.hpp"

namespace vz::fmt {

inline char hex_digit(unsigned v) {
    return v < 10 ? static_cast<char>('0' + v) : static_cast<char>('a' + v - 10);
}

/* Writes exactly `width` lowercase hex digits (low `width` nibbles of v).
   Caller supplies room for width chars; returns one past the last written. */
inline char* to_hex(char* dst, uint64_t v, unsigned width) {
    for (int i = static_cast<int>(width) - 1; i >= 0; --i) {
        dst[i] = hex_digit(static_cast<unsigned>(v & 0xF));
        v >>= 4;
    }
    return dst + width;
}

inline unsigned dec_width(uint64_t v) {
    unsigned n = 1;
    while (v >= 10) {
        v /= 10;
        ++n;
    }
    return n;
}

/* Writes decimal digits, no padding. Returns one past the last written. */
inline char* to_dec(char* dst, uint64_t v) {
    char tmp[20];
    unsigned n = 0;
    do {
        tmp[n++] = static_cast<char>('0' + (v % 10));
        v /= 10;
    } while (v != 0);
    for (unsigned i = 0; i < n; ++i) {
        dst[i] = tmp[n - 1 - i];
    }
    return dst + n;
}

} // namespace vz::fmt
