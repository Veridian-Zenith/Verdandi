#pragma once
#include <stdint.h>

namespace arch {

/* Instance-based 16550 UART driver: base address is runtime data, not a
   compile-time constant, so ACPI/PCI-discovered UARTs need no code change. */
class SerialPort {
  public:
    constexpr explicit SerialPort(uint16_t base) : base_(base) {}

    void init();
    void putc(char c);
    void write(const char* s);

  private:
    void outb(uint16_t off, uint8_t v) const;
    uint8_t inb(uint16_t off) const;

    uint16_t base_;
};

} // namespace arch
