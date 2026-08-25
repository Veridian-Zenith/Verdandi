#pragma once
#include <stdint.h>

#include "vz/types.hpp"

namespace arch {

class Gdt {
  public:
    static constexpr vz::u16 kSelCode = 1 * 8;
    static constexpr vz::u16 kSelData = 2 * 8;

    /* Null + ring-0 code64 (L=1 G=1) + writable data.
       Selector indices match the bootstrap GDT in entry.S by contract, so
       installing this table needs only data-segment reloads, no far jump. */
    void set_flat_lm();

    void install();

  private:
    vz::u64 entries_[3] = {};
};

} // namespace arch
