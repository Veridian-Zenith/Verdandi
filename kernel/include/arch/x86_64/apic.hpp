#pragma once
#include <stdint.h>

#include "vz/types.hpp"

namespace arch {

/* Legacy 8259A is silenced, never used: all lines masked after re-init. */
void mask_legacy_pic();

class Lapic {
  public:
    static constexpr vz::u8 kTimerVector = 0x20;

    bool probe();
    void enable();
    void start_timer(vz::u32 initial_count); /* periodic, divider /16 */
    void stop_timer();
    void send_eoi();

  private:
    vz::u32 read(vz::u32 offset) const;
    void write(vz::u32 offset, vz::u32 value);

    vz::u32 base_ = 0xFEE00000;
};

} // namespace arch
