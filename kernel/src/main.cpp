#include "arch/x86_64/apic.hpp"
#include "arch/x86_64/gdt.hpp"
#include "arch/x86_64/idt.hpp"
#include "arch/x86_64/serial.hpp"
#include "kernel/console.hpp"
#include "vz/fmt.hpp"
#include <stdint.h>

extern "C" void gate_div_run();
extern "C" void gate_div_fault_ip();
extern "C" void gate_div_resume_ip();

namespace {

struct GateCtx {
    vz::u64 fault_ip;
    vz::u64 resume_ip;
    bool recovered = false;
};

struct TickCtx {
    arch::Lapic* apic;
    vz::u32* count;
};

void on_tick(void* ctx, arch::Registers&) {
    auto* t = static_cast<TickCtx*>(ctx);
    ++*t->count;
    t->apic->send_eoi();
}

void put_dec(vz::u32 v) {
    char buf[10];
    *vz::fmt::to_dec(buf, v) = '\0';
    console::write(buf);
}

/* M1 gate: #DE prints a full register dump and resumes past the faulting
   div; any other #DE site halts via the default path. */
void gate_de_handler(void* ctx, arch::Registers& r) {
    auto* g = static_cast<GateCtx*>(ctx);
    if (r.rip != g->fault_ip) {
        arch::print_registers(r);
        console::write("[verdandi] #DE at unexpected rip, halting\n");
        arch::halt_forever();
    }
    arch::print_registers(r);
    r.rip = g->resume_ip;
    g->recovered = true;
}

} // namespace

extern "C" [[noreturn]] void kmain(uint32_t mb_info_phys) {
    (void)mb_info_phys; // parsed from M2 onward

    // Dependencies are constructed here and handed down; nothing owns globals.
    arch::SerialPort com1(0x3F8); // legacy UART; ACPI-discovery lands in M5
    com1.init();
    console::set_sink(
        {[](void* ctx, char c) { static_cast<arch::SerialPort*>(ctx)->putc(c); }, &com1});

    console::write("\n[verdandi] phase 2 alive: long mode, COM1 up\n");

    arch::Gdt gdt;
    gdt.set_flat_lm();
    gdt.install();

    arch::Idt idt;
    idt.reset();
    idt.set_exception_gates();
    idt.install();
    asm volatile("sti"); // legacy PIC lines masked; only our vectors arrive

    arch::Lapic lapic;
    vz::u32 ticks = 0;
    if (lapic.probe()) {
        arch::mask_legacy_pic();
        lapic.enable();
        TickCtx tick_ctx{&lapic, &ticks};
        idt.bind(arch::Lapic::kTimerVector, on_tick, &tick_ctx);
        lapic.start_timer(0x1000);
        while (ticks < 10) {
            asm volatile("hlt");
        }
        lapic.stop_timer();
        console::write("[verdandi] lapic timer: ");
        put_dec(ticks);
        console::write(" ticks observed\n");
    } else {
        console::write("[verdandi] no local APIC, skipping timer\n");
    }

    console::write("[verdandi] gate test: dividing by zero\n");
    GateCtx gate{reinterpret_cast<vz::u64>(&gate_div_fault_ip),
                 reinterpret_cast<vz::u64>(&gate_div_resume_ip)};
    idt.bind(0, gate_de_handler, &gate);
    gate_div_run();
    idt.unbind(0);

    if (gate.recovered) {
        console::write("[verdandi] recovered from #DE: M1 gate passed\n");
    } else {
        console::write("[verdandi] gate flag not set\n");
    }

    console::write("[verdandi] halting.\n");
    arch::halt_forever();
}
