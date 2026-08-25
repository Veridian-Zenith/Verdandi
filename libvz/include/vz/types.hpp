#pragma once
#include <stdint.h>

namespace vz {
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using uptr = uintptr_t;

static_assert(sizeof(u8) == 1 && sizeof(u32) == 4 && sizeof(uptr) >= 4);
} // namespace vz
