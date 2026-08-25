#include "arch/x86_64/serial.hpp"
#include "kernel/console.hpp"
#include <stdint.h>

extern "C" [[noreturn]] void kmain(uint32_t mb_info_phys) {
    (void)mb_info_phys;   // parsed from M1 onward

    // Dependencies are constructed here and handed down; nothing owns globals.
    arch::SerialPort com1(0x3F8);   // legacy UART; ACPI-discovery lands in M5
    com1.init();
    console::set_sink({
        [](void* ctx, char c) { static_cast<arch::SerialPort*>(ctx)->putc(c); },
        &com1
    });

    console::write("\n[verdandi] phase 0 alive: protected mode, COM1 up\n");
    console::write("[verdandi] halting. next milestone: M1\n");

    for (;;) {
        asm volatile("hlt");
    }
}
