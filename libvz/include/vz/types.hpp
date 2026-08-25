#pragma once
#include <stdint.h>

namespace vz {
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using uptr = uintptr_t;

static_assert(sizeof(u8) == 1 && sizeof(u32) == 4 && sizeof(uptr) >= 4);
} // namespace vz
